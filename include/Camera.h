#pragma once
#include <array>
#include <numbers>
#include <webgpu.h>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include <glm/glm.hpp>
#include <glm/ext.hpp>

struct MyUniforms {
	// We add transform matrices
    glm::mat4x4 projectionMatrix;
    glm::mat4x4 viewMatrix;
    glm::mat4x4 modelMatrix;
    std::array<float, 4> color;
    float time;
    float _pad[3];
};

class Camera
{
public:
	Camera();
	Camera(float eye_x, float eye_y, float eye_z, float lookat_x, float lookat_y, float loookat_z);
	~Camera();

	virtual void motion(int x, int y);
	virtual void release(int x, int y){}
	virtual void mouse(int button, int state, int x, int y);
	virtual void zoom(float dir);
	void resize(int w, int h);
	virtual void lookAt();
	virtual void update(WGPUQueue m_queue){
		wgpuQueueWriteBuffer(m_queue, m_uniform_buffer, offsetof(MyUniforms, viewMatrix), &m_uniforms.viewMatrix, sizeof(MyUniforms::viewMatrix));
	}

	glm::vec3 eye;
	glm::vec3 lookat;
	glm::vec3 up;

	unsigned int width, height;
	int last_x, last_y,cur_button;

private:
	glm::vec3  eye_distance;

public:
    float m_angle1;
    glm::mat4x4 m_R1, m_T1, m_S;
    MyUniforms m_uniforms;
	WGPUBuffer m_uniform_buffer;
	WGPUBuffer init(WGPUDevice device, WGPUQueue queue)
	{
		WGPUBufferDescriptor buffer_desc{
			.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
			.size = sizeof(MyUniforms),
			.mappedAtCreation = false,
		};
		m_uniform_buffer = wgpuDeviceCreateBuffer(device, &buffer_desc);



		// Translate the view
		glm::vec3 focalPoint(0.0, 0.0, .0);
		// Rotate the object
		m_angle1 = 2.0f; // arbitrary time

		// Rotate the view point
		float angle2 = 3.0f * std::numbers::pi / 4.0f;
		float ratio = 640.0f / 480.0f;
		float focalLength = 2.0;
		float near_depth = 0.01f;
		float far_depth = 100.0f;
		float divider = 1 / (focalLength * (far_depth - near_depth));

		// Option C: A different way of using GLM extensions
		glm::mat4x4 M(1.0);
		M = glm::rotate(M, m_angle1, glm::vec3(0.0, 0.0, 1.0));
		M = glm::translate(M, glm::vec3(0.5, 0.0, 0.0));
		M = glm::scale(M, glm::vec3(0.3f));
		m_uniforms.modelMatrix = M;

		glm::mat4x4 V(1.0);
		V = glm::translate(V, -focalPoint);
		V = glm::rotate(V, -angle2, glm::vec3(1.0, 0.0, 0.0));
		m_uniforms.viewMatrix = V;
		
		float fov = 2 * glm::atan(1 / focalLength);
		m_uniforms.projectionMatrix = glm::perspective(fov, ratio, near_depth, far_depth);

		m_uniforms.time = 1.0f;
		m_uniforms.color = { 0.0f, 1.0f, 0.4f, 1.0f };
		wgpuQueueWriteBuffer(queue, m_uniform_buffer, 0, &m_uniforms, sizeof(MyUniforms));

		return m_uniform_buffer;
	}
};