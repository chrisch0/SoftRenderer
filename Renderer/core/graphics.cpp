#include "graphics.h"
#include "math/math.h"

void PSInput::LerpAssgin(const PSInput& v0, const PSInput& v1, float t)
{
	sv_position = Lerp(v0.sv_position, v1.sv_position, t);
	normal = Lerp(v0.normal, v1.normal, t);
	positionWS = Lerp(v0.positionWS, v1.positionWS, t);
	uv = Lerp(v0.uv, v1.uv, t);
	color = Lerp(v0.color, v1.color, t);
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

void GraphicsContext::DrawIndexed(uint32_t indexCount, uint32_t startIndexLocation /* = 0 */, uint32_t baseVertexLocation /* = 0 */)
{
	auto num_faces = indexCount / 3;
	for (uint32_t face_idx = 0; face_idx < num_faces; ++face_idx)
	{
		m_numVertexIn = 0;
		m_numVertexOut = 0;
		// vertex shader stage
		for (int i = 0; i < 3; ++i)
		{
			VSInput* vs_input = &m_vertexBuffer[baseVertexLocation + m_indexBuffer[startIndexLocation + face_idx * 3 + i]];
			m_vertexListIn[i] = m_pipelineState->VS(vs_input, m_passConstant);
			m_numVertexIn++;
		}

		// triangle clipping
		for (int clipping_plane = 0; clipping_plane < Clipping_Plane_Count; ++clipping_plane)
		{
			m_numVertexOut = 0;
			for (int vertex_idx = 0; vertex_idx < m_numVertexIn; ++vertex_idx)
			{
				auto& last_vertex = m_vertexListIn[(vertex_idx - 1 + m_numVertexIn) % m_numVertexIn];
				auto& current_vertex = m_vertexListIn[vertex_idx];
				bool lv_inside = InsideClippingPlane((eHomoClippingPlane)clipping_plane, last_vertex.sv_position);
				bool cv_inside = InsideClippingPlane((eHomoClippingPlane)clipping_plane, current_vertex.sv_position);
				if (cv_inside)
				{
					// if need clip, add intersection point to out array
					if (!lv_inside)
					{
						float t = LineSegmentIntersectClippingPlane((eHomoClippingPlane)clipping_plane, last_vertex.sv_position, current_vertex.sv_position);
						VSOut& intersection_point = m_vertexListOut[m_numVertexOut];
						intersection_point.LerpAssgin(last_vertex, current_vertex, t);
						m_numVertexOut++;
					}
					m_vertexListOut[m_numVertexOut] = current_vertex;
					m_numVertexOut++;
				}
				else if (lv_inside)
				{
					float t = LineSegmentIntersectClippingPlane((eHomoClippingPlane)clipping_plane, last_vertex.sv_position, current_vertex.sv_position);
					VSOut& intersection_point = m_vertexListOut[m_numVertexOut];
					intersection_point.LerpAssgin(last_vertex, current_vertex, t);
					m_numVertexOut++;
				}
			}
			std::swap(m_vertexListIn, m_vertexListOut);
			m_numVertexIn = m_numVertexOut;
		}

		for (int v_idx = 0; v_idx < m_numVertexIn - 2; ++v_idx)
		{
			int idx0 = 0;
			int idx1 = v_idx + 1;
			int idx2 = v_idx + 2;

			// triangle assembly
			m_vertexListOut[0] = m_vertexListIn[idx0];
			m_vertexListOut[1] = m_vertexListIn[idx1];
			m_vertexListOut[2] = m_vertexListIn[idx2];

			// rasterize triangle

			// perspective division
			static float3 ndc_coords[3];
			static float recip_w[3];
			for (int i = 0; i < 3; ++i)
			{
				recip_w[i] = 1.f / m_vertexListOut[i].sv_position.w;
				m_vertexListOut[i].sv_position = m_vertexListOut[i].sv_position / m_vertexListOut[i].sv_position.w;
				ndc_coords[i] = float3(m_vertexListOut[i].sv_position);
			}

			// face culling
			if (m_pipelineState->RasterizerState.CullMode != Cull_Mode_None)
			{

				auto v0 = ndc_coords[0];
				auto v1 = ndc_coords[1];
				auto v2 = ndc_coords[2];
				float r = v0 * Cross(v1 - v0, v2 - v0);
				// front CCW
				bool is_back_face = !(r >= 0 ^ m_pipelineState->RasterizerState.FrontCounterClockWise);
				bool is_culling = !(m_pipelineState->RasterizerState.CullMode == Cull_Mode_Back ^ is_back_face);
				if (is_culling)
					continue;
			}

			// viewport mapping
			static float2 screen_coords[3];
			static float screen_depth[3];
			for (int i = 0; i < 3; ++i)
			{
				float3 ndc_coord = float3(m_vertexListOut[i].sv_position);
				float x = (ndc_coord.x + 1.f) * 0.5f * (float)m_frameBuffer->GetWidth() + m_viewport->TopLeftX;
				float y = (1.f - ndc_coord.y) * 0.5f * (float)m_frameBuffer->GetHeight() + m_viewport->TopLeftY;
				float z = m_viewport->MinDepth + ndc_coord.z * (m_viewport->MaxDepth - m_viewport->MinDepth);
				m_vertexListOut[i].sv_position = float4(x, y, z, 1.0f);
				screen_coords[i] = float2(x, y);
				screen_depth[i] = z;
			}

			// TODO: build bounding box

			// TODO: add wire frame rasterizer mode
	//#pragma omp parallel for schedule(dynamic)
			for (int x = 0; x < m_frameBuffer->GetWidth(); ++x)
			{
				for (int y = 0; y < m_frameBuffer->GetHeight(); ++y)
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

						// TODO: add early depth test
						// if (pipelineState.depthStencilState... && depth < depthBuffer[x][y])
						// interpolate vertex attributes
						PSInput pixel_attri;
						{
							float* a0 = (float*)&(m_vertexListOut[0]);
							float* a1 = (float*)&(m_vertexListOut[1]);
							float* a2 = (float*)&(m_vertexListOut[2]);
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
						Color pixel_color = m_pipelineState->PS(&pixel_attri, m_passConstant);

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

}

void GraphicsContext::ClearColor(FrameBuffer* frameBuffer, Color value)
{

}

void GraphicsContext::ClearColor(ColorBuffer* colorBuffer, Color value)
{

}