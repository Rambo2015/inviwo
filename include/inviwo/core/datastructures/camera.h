/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2012-2020 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#pragma once

#include <inviwo/core/common/inviwocoredefine.h>
#include <inviwo/core/io/serialization/serializable.h>
#include <inviwo/core/util/glm.h>

#include <memory>

namespace inviwo {

class CompositeProperty;

/**
 * \ingroup datastructures
 *
 * \brief Base class for cameras.
 * Override this class to set your own projection matrix.
 * @see PerspectiveCamera
 * @see OrthographicCamera
 */
class IVW_CORE_API Camera : public Serializable {
public:
    enum class Config { Show, Hide };

    /**
     * \brief Default parameters creates a right handed coordinate system
     * with camera looking towards the negative z-axis.
     * with X = (1, 0, 0), Y = (0, 1, 0), Z = (0, 0, -1)
     *
     * @param lookFrom Camera position (eye)
     * @param lookTo Camera focus point (center)
     * @param lookUp Camera up direction
     * @param nearPlane Camera near clip-plane
     * @param farPlane Camera far clip-plane
     */
    Camera(vec3 lookFrom = vec3(0.0f, 0.0f, 2.0f), vec3 lookTo = vec3(0.0f),
           vec3 lookUp = vec3(0.0f, 1.0f, 0.0f), float nearPlane = 0.01f,
           float farPlane = 10000.0f);
    virtual ~Camera() = default;
    Camera(const Camera& other) = default;
    Camera& operator=(const Camera& other) = default;

    virtual Camera* clone() const = 0;

    virtual bool update(const Camera* source) = 0;
    virtual void configureProperties(CompositeProperty* comp, Config config) = 0;

    const vec3& getLookFrom() const;
    virtual void setLookFrom(vec3 val);

    const vec3& getLookTo() const;
    virtual void setLookTo(vec3 val);

    const vec3& getLookUp() const;
    virtual void setLookUp(vec3 val);

    virtual float getAspectRatio() const = 0;
    virtual void setAspectRatio(float val) = 0;

    /**
     * \brief Get unnormalized direction of camera: lookTo - lookFrom
     */
    vec3 getDirection() const;

    /**
     * \brief Set distance to the near plane from lookFrom.
     */
    virtual void setNearPlaneDist(float distance);
    float getNearPlaneDist() const;

    /**
     * \brief Set distance to the far plane from lookFrom.
     */
    virtual void setFarPlaneDist(float distance);
    float getFarPlaneDist() const;

    const mat4& getViewMatrix() const;
    const mat4& getProjectionMatrix() const;
    const mat4& getInverseViewMatrix() const;
    const mat4& getInverseProjectionMatrix() const;

    /**
     * \brief Convert from normalized device coordinates (xyz in [-1 1]) to world coordinates.
     * @param ndcCoords Coordinates in [-1 1]
     * @return World space position
     */
    vec3 getWorldPosFromNormalizedDeviceCoords(const vec3& ndcCoords) const;

    /**
     * \brief Convert from normalized device coordinates (xyz in [-1 1]) to clip coordinates,
     * where z value of -1 correspond to the near plane and 1 to the far plane.
     * Coordinates outside of the [-1 1]^3 range will be clipped.
     *
     * @param ndcCoords xyz clip-coordinates in [-1 1]^3, and the clip w-coordinate used for
     * perspective division.
     * @return Clip space position
     */
    virtual vec4 getClipPosFromNormalizedDeviceCoords(const vec3& ndcCoords) const;

    vec3 getNormalizedDeviceFromNormalizedScreenAtFocusPointDepth(
        const vec2& normalizedScreenCoord) const;

    virtual void serialize(Serializer& s) const override;
    virtual void deserialize(Deserializer& d) override;

    friend IVW_CORE_API bool operator==(const Camera& lhs, const Camera& rhs) {
        return lhs.equal(rhs);
    }
    friend IVW_CORE_API bool operator!=(const Camera& lhs, const Camera& rhs) {
        return !(lhs == rhs);
    }

protected:
    virtual bool equal(const Camera& other) const = 0;
    bool equalTo(const Camera& other) const;

    /**
     * \brief Calculate and return the projection matrix for the camera.
     *
     * Implement this function to provide your own projection computation functionality.
     * For example orthogonal or perspective projection.
     * This function will be called when the projection matrix is invalid.
     * @see PerspectiveCamera
     * @see OrthographicCamera
     */
    virtual mat4 calculateProjectionMatrix() const = 0;

    virtual mat4 calculateViewMatrix() const;

    void invalidateViewMatrix();
    void invalidateProjectionMatrix();

    vec3 lookFrom_;
    vec3 lookTo_;
    vec3 lookUp_;

    float nearPlaneDist_;  ///< Distance to the near plane from lookFrom.
    float farPlaneDist_;   ///< Distance to the far plane from lookFrom.

    // Make mutable to allow then to be changed even though they are called from const function.
    // This allows us to perform lazy evaluation.
    mutable bool invalidViewMatrix_;
    mutable bool invalidProjectionMatrix_;
    mutable mat4 viewMatrix_;
    mutable mat4 projectionMatrix_;
    mutable mat4 inverseViewMatrix_;
    mutable mat4 inverseProjectionMatrix_;
};

class IVW_CORE_API PerspectiveCamera final : public Camera {
public:
    PerspectiveCamera(vec3 lookFrom = vec3(0.0f, 0.0f, 2.0f), vec3 lookTo = vec3(0.0f),
                      vec3 lookUp = vec3(0.0f, 1.0f, 0.0f), float nearPlane = 0.01f,
                      float farPlane = 10000.0f, float fieldOfView = 60.f, float aspectRatio = 1.f);
    virtual ~PerspectiveCamera() = default;
    PerspectiveCamera(const PerspectiveCamera& other);
    PerspectiveCamera& operator=(const PerspectiveCamera& other);
    virtual PerspectiveCamera* clone() const override;
    virtual bool update(const Camera* source) override;
    virtual void configureProperties(CompositeProperty* comp, Config config) override;

    float getFovy() const;
    void setFovy(float val);
    virtual float getAspectRatio() const override;
    virtual void setAspectRatio(float val) override;

    virtual void serialize(Serializer& s) const override;
    virtual void deserialize(Deserializer& d) override;

protected:
    virtual bool equal(const Camera& other) const override;
    virtual mat4 calculateProjectionMatrix() const override;

    float fovy_;
    float aspectRatio_;
    std::shared_ptr<std::function<void()>> fovCallbackHolder_;
};

/**
 * \class OrthographicCamera
 *
 * \brief Camera with no perspective projection.
 * Objects far away will appear as large as objects close.
 * @see Camera
 * @see OrthographicCamera
 */
class IVW_CORE_API OrthographicCamera final : public Camera {
public:
    OrthographicCamera(vec3 lookFrom = vec3(0.0f, 0.0f, 2.0f), vec3 lookTo = vec3(0.0f),
                       vec3 lookUp = vec3(0.0f, 1.0f, 0.0f), float nearPlane = 0.01f,
                       float farPlane = 10000.0f, float width = 10.f, float aspectRatio = 1.f);
    virtual ~OrthographicCamera() = default;
    OrthographicCamera(const OrthographicCamera& other);
    OrthographicCamera& operator=(const OrthographicCamera& other);
    virtual OrthographicCamera* clone() const override;
    virtual bool update(const Camera* source) override;
    virtual void configureProperties(CompositeProperty* comp, Config config) override;

    float getWidth() const;
    void setWidth(float width);
    virtual float getAspectRatio() const override;
    virtual void setAspectRatio(float val) override;

    virtual vec4 getClipPosFromNormalizedDeviceCoords(const vec3& ndcCoords) const override;

    virtual void serialize(Serializer& s) const override;
    virtual void deserialize(Deserializer& d) override;

protected:
    virtual bool equal(const Camera& other) const override;
    virtual mat4 calculateProjectionMatrix() const override;

    float aspectRatio_;
    float width_;
    std::shared_ptr<std::function<void()>> widthCallbackHolder_;
};

/**
 * \brief Camera with off axis perspective projection.
 *
 * The camera with unsymmetrical frustum for stereo in VR
 * Kooima, Robert. "Generalized perspective projection." School of Elect. Eng. and Computer Science
 * (2008): 1-7.
 * @see Camera
 * @see SkewedPerspectiveCamera
 */
class IVW_CORE_API SkewedPerspectiveCamera final : public Camera {
public:
    SkewedPerspectiveCamera(vec3 lookFrom = vec3(0.0f, 0.0f, 2.0f), vec3 lookTo = vec3(0.0f),
                            vec3 lookUp = vec3(0.0f, 1.0f, 0.0f), float nearPlane = 0.01f,
                            float farPlane = 10000.0f, float fieldOfView = 60.f,
                            float aspectRatio = 1.f, vec2 frustumOffset = vec2(0.0f, 0.0f));
    virtual ~SkewedPerspectiveCamera() = default;
    SkewedPerspectiveCamera(const SkewedPerspectiveCamera& other);
    SkewedPerspectiveCamera& operator=(const SkewedPerspectiveCamera& other);
    virtual SkewedPerspectiveCamera* clone() const override;
    virtual bool update(const Camera* source) override;
    virtual void configureProperties(CompositeProperty* comp, Config config) override;

    float getFovy() const;
    void setFovy(float val);

    virtual void setLookFrom(vec3 val) override;
    virtual void setLookTo(vec3 val) override;

    virtual float getAspectRatio() const override;
    virtual void setAspectRatio(float val) override;
    const vec2& getOffset() const;
    void setOffset(vec2 val);

    virtual mat4 calculateViewMatrix() const override;

    virtual void serialize(Serializer& s) const override;
    virtual void deserialize(Deserializer& d) override;

protected:
    virtual bool equal(const Camera& other) const override;
    virtual mat4 calculateProjectionMatrix() const override;

    // Left, right, bottom, top view volume
    float fovy_;
    float aspectRatio_;
    vec2 offset_;
    std::shared_ptr<std::function<void()>> fovCallbackHolder_;
    std::shared_ptr<std::function<void()>> offsetCallbackHolder_;
};

// Implementation details
inline const vec3& Camera::getLookFrom() const { return lookFrom_; }
inline const vec3& Camera::getLookTo() const { return lookTo_; }
inline const vec3& Camera::getLookUp() const { return lookUp_; }

inline void Camera::setLookFrom(vec3 val) {
    lookFrom_ = val;
    invalidateViewMatrix();
}

inline void Camera::setLookTo(vec3 val) {
    lookTo_ = val;
    invalidateViewMatrix();
}

inline void Camera::setLookUp(vec3 val) {
    lookUp_ = val;
    invalidateViewMatrix();
}

inline vec3 Camera::getDirection() const { return lookTo_ - lookFrom_; }

inline float Camera::getNearPlaneDist() const { return nearPlaneDist_; }
inline void Camera::setNearPlaneDist(float val) {
    nearPlaneDist_ = val;
    invalidateProjectionMatrix();
}
inline float Camera::getFarPlaneDist() const { return farPlaneDist_; }
inline void Camera::setFarPlaneDist(float val) {
    farPlaneDist_ = val;
    invalidateProjectionMatrix();
}

inline void Camera::invalidateViewMatrix() { invalidViewMatrix_ = true; }
inline void Camera::invalidateProjectionMatrix() { invalidProjectionMatrix_ = true; }

inline float PerspectiveCamera::getFovy() const { return fovy_; }
inline void PerspectiveCamera::setFovy(float val) {
    fovy_ = val;
    invalidateProjectionMatrix();
}
inline float PerspectiveCamera::getAspectRatio() const { return aspectRatio_; }
inline void PerspectiveCamera::setAspectRatio(float val) {
    aspectRatio_ = val;
    invalidateProjectionMatrix();
}



inline mat4 PerspectiveCamera::calculateProjectionMatrix() const {
    return glm::perspective(glm::radians(fovy_), aspectRatio_, nearPlaneDist_, farPlaneDist_);
}

inline float OrthographicCamera::getAspectRatio() const { return aspectRatio_; }

inline void OrthographicCamera::setAspectRatio(float val) {
    aspectRatio_ = val;
    invalidateProjectionMatrix();
}

inline float OrthographicCamera::getWidth() const { return width_; }

inline void OrthographicCamera::setWidth(float width) {
    width_ = width;
    invalidateProjectionMatrix();
}

inline float SkewedPerspectiveCamera::getFovy() const { return fovy_; }
inline void SkewedPerspectiveCamera::setFovy(float val) {
    fovy_ = val;
    invalidateProjectionMatrix();
}
inline void SkewedPerspectiveCamera::setLookFrom(vec3 val) {
    Camera::setLookFrom(val);
    invalidateProjectionMatrix();
}
inline void SkewedPerspectiveCamera::setLookTo(vec3 val) {
    Camera::setLookTo(val);
    invalidateProjectionMatrix();
}

inline float SkewedPerspectiveCamera::getAspectRatio() const { return aspectRatio_; }
inline void SkewedPerspectiveCamera::setAspectRatio(float val) {
    aspectRatio_ = val;
    invalidateProjectionMatrix();
}

inline const vec2& SkewedPerspectiveCamera::getOffset() const { return offset_; }
inline void SkewedPerspectiveCamera::setOffset(vec2 offset) {
    offset_ = offset;
    invalidateViewMatrix();
    invalidateProjectionMatrix();
}

}  // namespace inviwo
