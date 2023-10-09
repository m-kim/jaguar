#include "Camera.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <cmath>
#include <iostream>

using namespace std;

Camera::Camera()
{
	eye = {0,0,1};
	lookat = {0,0,0};
	up = {0,1,0};
	width = 0;
	height = 0;

}
Camera::Camera(float eye_x, float eye_y, float eye_z, float lookat_x, float lookat_y, float lookat_z)
{
	// initialize the camera
    eye = {eye_x, eye_y, eye_z};
    lookat = {lookat_x, lookat_y, lookat_z};
    up = {0,1,0};
    cur_button = -1;
}

Camera::~Camera()
{
}
void Camera::motion(int x, int y)
{
	// the change in mouse position
		int dx = x-last_x;
		int dy = y-last_y;

		float scale, len, thetaxz, thetay;
		glm::vec3 f, r, u,neye, neye2;
		switch(cur_button)
		{
		case GLFW_MOUSE_BUTTON_RIGHT:
			// translate
			f = lookat - eye;
			u.x = 0;
			u.y = 1;
			u.z = 0;

			// scale the change by how far away we are
			scale = sqrt(f.length()) * 0.007;


	        r = glm::cross(f, u);
	        u = glm::cross(r, f);
	        r = glm::normalize(r);
	        u = glm::normalize(u);
	        r *= -(dx * scale);
	        u *= dy * scale;
			r += u;
			eye += r;
			lookat += r;
            lookAt();
			break;

		case GLFW_MOUSE_BUTTON_MIDDLE:


		case GLFW_MOUSE_BUTTON_LEFT:
			neye = eye - lookat;

			// first rotate in the x/z plane
			thetaxz = -dx * 0.007;
			neye2.x = (float)cos(thetaxz)*neye.x + (float)sin(thetaxz)*neye.z;
			neye2.y = neye.y;
			neye2.z =-(float)sin(thetaxz)*neye.x + (float)cos(thetaxz)*neye.z;

			// now rotate vertically
			thetay = -dy * 0.007;

			f = neye2 * -1.0f;
			u.x = 0;
			u.y = 1;
			u.z = 0;
	        r = glm::cross(f, u);
			u = glm::cross(r, f);
			len = glm::length(f);
			f = glm::normalize(f);
			u = glm::normalize(u);

	        f *= (float)cos(thetay);
	        u *= (float)sin(thetay);
			neye = ( f  + u ) * len;
			eye = lookat - neye;

            lookAt();
			break;

		}
		last_x = x;
		last_y = y;
}

void Camera::mouse(int button, int state, int x, int y)
{

	if (state == GLFW_PRESS)
    {
		if (button == 3 ){
			zoom(y*2);
        }
		else{
			cur_button = button;

		}
        lookAt();
	}
	else
	{
		if (button == cur_button)
			cur_button = -1;
	}
	last_x = x;
	last_y = y;
}

void Camera::resize(int w, int h)
{
	width = w;
	height = h;
}

void Camera::lookAt()
{
	//gluLookAt(eye.x, eye.y, eye.z, lookat.x, lookat.y, lookat.z, up.x, up.y, up.z);
    m_uniforms.viewMatrix = glm::lookAt(eye, lookat, up);
}
// void Camera::postLookAt()
// {
// 	glPopMatrix();
// }

void Camera::zoom(float dir)
{
	float scale, len, thetaxz, thetay;
	glm::vec3 f, r, u,neye, neye2;
	// zoom
	f = lookat - eye;
	len = glm::length(f);
	f = glm::normalize(f);
	len -= sqrt(len)* dir * 0.03;

	u = f*len;
	eye = lookat - u;

	eye_distance = eye - lookat;
}
