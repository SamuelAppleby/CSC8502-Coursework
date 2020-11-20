#include "Camera.h"
#include "../nclgl/Window.h"

void Camera::UpdateCamera(float dt) {
	pitch -= (Window::GetMouse()->GetRelativePosition().y);
	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);
	yaw -= (Window::GetMouse()->GetRelativePosition().x);
	if (yaw < 0) {
		yaw += 360.0f;
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_Z)) {
		roll += velocity;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_X)) {
		roll -= velocity;
	}
	if (roll < 0) {
		roll += 360.0f;
	}
	if (roll > 360.0f) {
		roll -= 360.0f;
	}
	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));
	Vector3 forward = rotation * Vector3(0, 0, -1);
	Vector3 right = rotation * Vector3(1, 0, 0);
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
		position += forward * velocity * dt;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
		position -= forward * velocity * dt;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
		position -= right * velocity * dt;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
		position += right * velocity * dt;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
		position.y += velocity * dt;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
		position.y -= velocity * dt;
	}
}
Matrix4 Camera::BuildViewMatrix() {
	return Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) * Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) 
		* Matrix4::Rotation(-roll, Vector3(0, 0, 1)) * Matrix4::Translation(-position);
}
void Camera::MoveLeft(float dt) {
	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));
	Vector3 right = rotation * Vector3(1, 0, 0);
	position -= right * (velocity * dt);
}
void Camera::MoveRight(float dt) {
	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));
	Vector3 right = rotation * Vector3(1, 0, 0);
	position += right * (velocity * dt);
}
void Camera::MoveForward(float dt) {
	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));
	Vector3 forward = rotation * Vector3(0, 0, -1);
	position += forward * (velocity * dt);
}
void Camera::MoveBack(float dt) {
	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));
	Vector3 forward = rotation * Vector3(0, 0, -1);
	position -= forward * (velocity * dt);
}
void Camera::MoveUp(float dt) {
	position.y += (velocity * dt);
}
void Camera::MoveDown(float dt) {
	position.y -= (velocity * dt);
}
void Camera::RollRight(float dt) {
	roll++;
	if (roll > 360.0f) {
		roll -= 360.0f;
	}
}
void Camera::RollLeft(float dt) {
	roll--;
	if (roll < 0) {
		roll += 360.0f;
	}
}
void Camera::LookLeft(float dt) {
	yaw += (velocity * dt);
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}
}
void Camera::LookRight(float dt) {
	yaw -= (velocity * dt);
	if (yaw < 0) {
		yaw += 360.0f;
	}
}
void Camera::LookUp(float dt) {
	pitch += (velocity * dt);
	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);
}
void Camera::LookDown(float dt) {
	pitch -= (velocity * dt);
	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);
}