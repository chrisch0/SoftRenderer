#include "graphics.h"
#include "math/math.h"

void PSInput::LerpAssgin(const PSInput& v0, const PSInput& v1, float t)
{
	sv_position = Lerp(v0.sv_position, v1.sv_position, t);
	normal = Lerp(v0.normal, v1.normal, t);
	positionWS = Lerp(v0.positionWS, v1.positionWS, t);
	uv = Lerp(v0.uv, v1.uv, t);
	color = Lerp(v0.color, v1.color, t);
	tangent = Lerp(v0.tangent, v1.tangent, t);
	bitangent = Lerp(v0.bitangent, v1.bitangent, t);
}

bool InsideClippingPlane(eHomoClippingPlane plane, const float4& coord);

float LineSegmentIntersectClippingPlane(eHomoClippingPlane plane, const float4& p0, const float4& p1);

int TriangleClipping(std::array<VSOut, 10>& VSOutVertices, std::array<PSInput, 10>& PSInVertices);

bool DepthTest(eDepthFunc testFunc, float curDepth, float prevDepth);

void GraphicsContext::DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation /* = 0 */, uint32_t baseVertexLocation /* = 0 */)
{
	auto num_faces = indexCount / 3;
#pragma omp parallel for schedule(dynamic)
	for (int face_idx = 0; face_idx < num_faces; ++face_idx)
	{
		std::array<VSOut, 10> vs_out_vertices;
		std::array<PSInput, 10> ps_in_vertices;
		// vertex shader stage
		for (int i = 0; i < 3; ++i)
		{
			VSInput* vs_input = &m_vertexBuffer[baseVertexLocation + m_indexBuffer[startIndexLocation + face_idx * 3 + i]];
			vs_out_vertices[i] = m_pipelineState->VS(vs_input, m_constantBuffer);
		}

		// triangle clipping
		int num_ps_in = TriangleClipping(vs_out_vertices, ps_in_vertices);

		for (int v_idx = 0; v_idx < num_ps_in - 2; ++v_idx)
		{
			int idx0 = 0;
			int idx1 = v_idx + 1;
			int idx2 = v_idx + 2;

			// triangle assembly
			ps_in_vertices[0] = vs_out_vertices[idx0];
			ps_in_vertices[1] = vs_out_vertices[idx1];
			ps_in_vertices[2] = vs_out_vertices[idx2];

			// rasterize triangle
			// perspective division
			float3 ndc_coords[3];
			float recip_w[3];
			for (int i = 0; i < 3; ++i)
			{
				recip_w[i] = 1.f / ps_in_vertices[i].sv_position.w;
				ps_in_vertices[i].sv_position = ps_in_vertices[i].sv_position / ps_in_vertices[i].sv_position.w;
				ndc_coords[i] = float3(ps_in_vertices[i].sv_position);
			}

			// face culling
			if (m_pipelineState->RasterizerState.CullMode != Cull_Mode_None)
			{
				auto v0 = ndc_coords[0];
				auto v1 = ndc_coords[1];
				auto v2 = ndc_coords[2];
				float r = Dot(v0, Cross(v1 - v0, v2 - v0));

				bool is_back_face = !(r < 0 ^ m_pipelineState->RasterizerState.FrontCounterClockWise);
				bool is_culling = !(m_pipelineState->RasterizerState.CullMode == Cull_Mode_Back ^ is_back_face);
				if (is_culling)
					continue;
			}

			// viewport mapping
			float2 screen_coords[3];
			float screen_depth[3];
			for (int i = 0; i < 3; ++i)
			{
				float3 ndc_coord = float3(ps_in_vertices[i].sv_position);
				float x = (ndc_coord.x + 1.f) * 0.5f * (float)m_frameBuffer->GetWidth() + m_viewport->TopLeftX;
				float y = (1.f - ndc_coord.y) * 0.5f * (float)m_frameBuffer->GetHeight() + m_viewport->TopLeftY;
				float z = m_viewport->MinDepth + ndc_coord.z * (m_viewport->MaxDepth - m_viewport->MinDepth);
				ps_in_vertices[i].sv_position = float4(x, y, z, 1.0f);
				screen_coords[i] = float2(x, y);
				screen_depth[i] = z;
			}

			// build bounding box
			float2 range_min = Min(screen_coords[0], Min(screen_coords[1], screen_coords[2]));
			float2 range_max = Max(screen_coords[0], Max(screen_coords[1], screen_coords[2]));
			int x_min = (int)std::floor(range_min.x);
			int y_min = (int)std::floor(range_min.y);
			int x_max = (int)std::ceil(range_max.x);
			int y_max = (int)std::ceil(range_max.y);
			x_min = std::max(x_min, 0);
			x_max = std::min(x_max, m_frameBuffer->GetWidth());
			y_min = std::max(y_min, 0);
			y_max = std::min(y_max, m_frameBuffer->GetHeight());

			// TODO: add wire frame rasterizer mode
	//#pragma omp parallel for schedule(dynamic)
			for (int x = x_min; x < x_max; ++x)
			{
				for (int y = y_min; y < y_max; ++y)
				{
					float2 point = float2((float)x + 0.5f, (float)y + 0.5f);
					float3 weights;
					{
						float2 a = screen_coords[0];
						float2 b = screen_coords[1];
						float2 c = screen_coords[2];
						float2 bp = point - b;
						float2 bc = c - b;
						float2 ba = a - b;
						float2 cp = point - c;
						float2 ca = a - c;
						float alpha = (-bp.x * bc.y + bp.y * bc.x) / (-ba.x * bc.y + ba.y * bc.x);
						float beta = (-cp.x * ca.y + cp.y * ca.x) / (bc.x * ca.y - bc.y * ca.x);
						weights = float3(alpha, beta, 1 - alpha - beta);
					}
					// if pixel inside triangle
					if (weights.x > -std::numeric_limits<float>::epsilon() &&
						weights.y > -std::numeric_limits<float>::epsilon() &&
						weights.z > -std::numeric_limits<float>::epsilon())
					{
						// interpolate depth
						float depth = screen_depth[0] * weights.x + screen_depth[1] * weights.y + screen_depth[2] * weights.z;

						// early depth test
						if (m_depthBuffer != nullptr && m_pipelineState->DepthStencilState.DepthEnable)
						{
							float prev_depth = m_depthBuffer->GetValue(x, y);
							if (!DepthTest(m_pipelineState->DepthStencilState.DepthFunc, depth, prev_depth))
							{
								continue;
							}
							else
							{
								m_depthBuffer->SetValue(x, y, depth);
							}
						}

						// interpolate vertex attributes
						PSInput pixel_attri;
						{
							float* a0 = (float*)&(ps_in_vertices[0]);
							float* a1 = (float*)&(ps_in_vertices[1]);
							float* a2 = (float*)&(ps_in_vertices[2]);
							float* r = (float*)&pixel_attri;
							float weight0 = recip_w[0] * weights.x;
							float weight1 = recip_w[1] * weights.y;
							float weight2 = recip_w[2] * weights.z;
							float norm = 1.f / (weight0 + weight1 + weight2);
							// perspective correct interpolation
							for (int j = 0; j < sizeof(PSInput) / sizeof(float); ++j)
							{
								float attri = norm * (a0[j] * weight0 + a1[j] * weight1 + a2[j] * weight2);
								r[j] = attri;
							}
						}
						// TODO: multiple render targets
						// pixel shader stage
						Color pixel_color = m_pipelineState->PS(&pixel_attri, m_constantBuffer, m_textureSlots, m_samplerSlots);

						// TODO:: add blend
						m_frameBuffer->SetColorBGR(x, y, pixel_color);
					}
				}
			}
		}
	}
}

void GraphicsContext::ClearDepth(DepthBuffer* depthBuffer, float value)
{
	int width = depthBuffer->GetWidth();
	int height = depthBuffer->GetHeight();
#pragma omp parallel for schedule(dynamic)
	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			int index = width * y * DepthBuffer::channels + x * DepthBuffer::channels;
			depthBuffer->SetValue(index, value);
		}
	}
}

void GraphicsContext::ClearColor(FrameBuffer* frameBuffer, const Color& value)
{
	int width = frameBuffer->GetWidth();
	int height = frameBuffer->GetHeight();
#pragma omp parallel for schedule(dynamic)
	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			int index = width * y * FrameBuffer::channels + x * FrameBuffer::channels;
			frameBuffer->SetValue(index, std::clamp(value.z, 0.0f, 1.0f) * 255);
			frameBuffer->SetValue(index + 1, std::clamp(value.y, 0.0f, 1.0f) * 255);
			frameBuffer->SetValue(index + 2, std::clamp(value.x, 0.0f, 1.0f) * 255);
		}
	}
}

void GraphicsContext::ClearColor(ColorBuffer* colorBuffer, const Color& value)
{
	int width = colorBuffer->GetWidth();
	int height = colorBuffer->GetHeight();
#pragma omp parallel for schedule(dynamic)
	for (int x = 0; x < width; ++x)
	{
		for (int y = 0; y < height; ++y)
		{
			int index = width * y * ColorBuffer::channels + x * ColorBuffer::channels;
			colorBuffer->SetValue(index, value.x);
			colorBuffer->SetValue(index, value.y);
			colorBuffer->SetValue(index, value.z);
			colorBuffer->SetValue(index, value.w);
		}
	}
}


bool InsideClippingPlane(eHomoClippingPlane plane, const float4& coord)
{
	switch (plane)
	{
	case Positive_W:
		return coord.w >= 1e-5f;
	case Positive_X:
		return coord.x <= coord.w;
	case Negative_X:
		return coord.x >= -coord.w;
	case Positive_Y:
		return coord.y <= coord.w;
	case Negative_Y:
		return coord.y >= -coord.w;
	case Positive_Z:
		return coord.z <= coord.w;
	case Negative_Z:
		return coord.z >= 1e-5f;
	default:
		return false;
	}
}

float LineSegmentIntersectClippingPlane(eHomoClippingPlane plane, const float4& p0, const float4& p1)
{
	switch (plane)
	{
	case Positive_W:
		return (p0.w - 1e-5f) / (p0.w - p1.w);
	case Positive_X:
		return (p0.w - p0.x) / ((p0.w - p0.x) - (p1.w - p1.x));
	case Negative_X:
		return (p0.w + p0.x) / ((p0.w + p0.x) - (p1.w + p1.x));
	case Positive_Y:
		return (p0.w - p0.y) / ((p0.w - p0.y) - (p1.w - p1.y));
	case Negative_Y:
		return (p0.w + p0.y) / ((p0.w + p0.y) - (p1.w + p1.y));
	case Positive_Z:
		return (p0.w - p0.z) / ((p0.w - p0.z) - (p1.w - p1.z));
	case Negative_Z:
		return (p0.z - 1e-5f) / (p0.z - p1.z);
	default:
		return 0.0f;
	}
}

int TriangleClipping(std::array<VSOut, 10>& VSOutVertices, std::array<PSInput, 10>& PSInVertices)
{
	int numVSOutVertices = 3;
	int numPSInVertices;

	for (int clipping_plane = 0; clipping_plane < Clipping_Plane_Count; ++clipping_plane)
	{
		numPSInVertices = 0;
		for (int vertex_idx = 0; vertex_idx < numVSOutVertices; ++vertex_idx)
		{
			auto& last_vertex = VSOutVertices[(vertex_idx - 1 + numVSOutVertices) % numVSOutVertices];
			auto& current_vertex = VSOutVertices[vertex_idx];
			bool lv_inside = InsideClippingPlane((eHomoClippingPlane)clipping_plane, last_vertex.sv_position);
			bool cv_inside = InsideClippingPlane((eHomoClippingPlane)clipping_plane, current_vertex.sv_position);
			if (cv_inside)
			{
				// if need clip, add intersection point to out array
				if (!lv_inside)
				{
					float t = LineSegmentIntersectClippingPlane((eHomoClippingPlane)clipping_plane, last_vertex.sv_position, current_vertex.sv_position);
					VSOut& intersection_point = PSInVertices[numPSInVertices];
					intersection_point.LerpAssgin(last_vertex, current_vertex, t);
					numPSInVertices++;
				}
				PSInVertices[numPSInVertices] = current_vertex;
				numPSInVertices++;
			}
			else if (lv_inside)
			{
				float t = LineSegmentIntersectClippingPlane((eHomoClippingPlane)clipping_plane, last_vertex.sv_position, current_vertex.sv_position);
				VSOut& intersection_point = PSInVertices[numPSInVertices];
				intersection_point.LerpAssgin(last_vertex, current_vertex, t);
				numPSInVertices++;
			}
		}
		std::swap(VSOutVertices, PSInVertices);
		numVSOutVertices = numPSInVertices;
	}
	return numPSInVertices;
}

bool DepthTest(eDepthFunc testFunc, float curDepth, float prevDepth)
{
	switch (testFunc)
	{
	case Comparison_Func_Never:
		return false;
	case Comparison_Func_Less:
		return curDepth < prevDepth;
	case Comparison_Func_Equal:
		return curDepth == prevDepth;
	case Comparison_Func_Less_Equal:
		return curDepth <= prevDepth;
	case Comparison_Func_Greater:
		return curDepth > prevDepth;
	case Comparison_Func_Not_Equal:
		return curDepth != prevDepth;
	case Comparison_Func_Greater_Equal:
		return curDepth >= prevDepth;
	case Comparison_Func_Always:
		return true;
	default:
		return false;
	}
}