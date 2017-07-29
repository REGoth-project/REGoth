#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/norm.hpp>
#include <string.h>
#include <iostream>
#include <string>

namespace Math
{
    static const float PI = glm::pi<float>();

    constexpr int64_t ipow(int64_t base, int exp, int64_t result = 1)
    {
        return exp < 1 ? result : ipow(base * base, exp / 2, (exp % 2) ? result * base : result);
    }

    /**
     * Truncate a floating-point number (Remove everything after the decimal-point))
     * @param f Float to truncate
     * @return f, but only the integer part
     */
    inline int trunc(float f)
    {
        return static_cast<int>(f);
    }

    /**
     * Rounds a number to the nearest decimal
     * @param f number to round
     * @return f, but rounded to the integer value of f
     */
    inline int iround(float f)
    {
        return static_cast<int>(f + 0.5f);
    }

    /**
     * Lower next integer value of f
     * @param f Number to process
     * @return Lower next integer value of f
     */
    inline int ifloor(float f)
    {
        return round(floor(f));
    }

    /**
     * Upper next integer value of f
     * @param f Number to process
     * @return Upper next integer value of f
     */
    inline int iceil(float f)
    {
        return round(ceil(f));
    }

    /**
     * @brief Converson of degrees to radians
     */
    static float DegToRad(float deg)
    {
        return glm::radians(deg);
    }

    struct t_float2
    {
        t_float2() {}
        t_float2(float x, float y)
        {
            this->x = x;
            this->y = y;
        }

        union {
            struct
            {
                float x;
                float y;
            };

            float v[2];
            glm::vec2 _glmt_vector;
        };

        std::string toString()
        {
            std::string out;
            out = "[" + std::to_string(x) + ", " + std::to_string(y) + "]";

            return out;
        }

        friend std::ostream& operator<<(std::ostream& out, t_float2& v);
    };

    std::ostream& operator<<(std::ostream& out, t_float2& v);

    struct t_float3
    {
        t_float3() {}
        t_float3(float x, float y, float z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        union {
            struct
            {
                float x;
                float y;
                float z;
            };

            float v[3];
            glm::vec3 _glmt_vector;
        };

        std::string toString() const
        {
            std::string out;
            out = "[" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + "]";

            return out;
        }

        friend std::ostream& operator<<(std::ostream& out, t_float3& v);
    };

    std::ostream& operator<<(std::ostream& out, t_float3& v);

    struct t_float4
    {
        t_float4() {}
        t_float4(float x, float y, float z, float w)
        {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
        }

        union {
            struct
            {
                float x;
                float y;
                float z;
                float w;
            };

            float v[4];

            glm::vec4 _glmt_vector;
            glm::quat _glmt_quat;
        };

        /**
		 * @brief Converts the given ABGR8-Color to float4
		 */
        void fromABGR8(uint32_t abgr)
        {
            unsigned char a = static_cast<unsigned char>((abgr >> 24) & 0xFF);
            unsigned char b = static_cast<unsigned char>((abgr >> 16) & 0xFF);
            unsigned char g = static_cast<unsigned char>((abgr >> 8) & 0xFF);
            unsigned char r = static_cast<unsigned char>(abgr & 0xFF);

            x = r / 255.0f;
            y = g / 255.0f;
            z = b / 255.0f;
            w = a / 255.0f;
        }

        void fromRGBA8(uint32_t rgba)
        {
            unsigned char r = static_cast<unsigned char>(rgba >> 24 & 0xFF);
            unsigned char g = static_cast<unsigned char>((rgba >> 16) & 0xFF);
            unsigned char b = static_cast<unsigned char>((rgba >> 8) & 0xFF);
            unsigned char a = static_cast<unsigned char>(rgba & 0xFF);

            x = r / 255.0f;
            y = g / 255.0f;
            z = b / 255.0f;
            w = a / 255.0f;
        }

        /**
		* @brief Converts the stored color to ABGR8
		*/
        uint32_t toABGR8() const
        {
            unsigned char b[] = {static_cast<unsigned char>(w * 255.0f),
                                 static_cast<unsigned char>(z * 255.0f),
                                 static_cast<unsigned char>(y * 255.0f),
                                 static_cast<unsigned char>(x * 255.0f)};

            return *reinterpret_cast<uint32_t*>(b);
        }

        /**
		* @brief Converts the stored color to ARGB8
		*/
        uint32_t toARGB8() const
        {
            unsigned char b[] = {static_cast<unsigned char>(w * 255.0f),
                                 static_cast<unsigned char>(x * 255.0f),
                                 static_cast<unsigned char>(y * 255.0f),
                                 static_cast<unsigned char>(z * 255.0f)};

            return *reinterpret_cast<uint32_t*>(b);
        }

        /**
		* @brief Converts the stored color to RGBA8
		*/
        uint32_t toRGBA8() const
        {
            unsigned char b[] = {static_cast<unsigned char>(x * 255.0f),
                                 static_cast<unsigned char>(y * 255.0f),
                                 static_cast<unsigned char>(z * 255.0f),
                                 static_cast<unsigned char>(w * 255.0f)};

            return *reinterpret_cast<uint32_t*>(b);
        }

        std::string toString()
        {
            std::string out;
            out = "[" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ", " + std::to_string(w) + "]";

            return out;
        }

        static t_float4 slerp(const t_float4& a, const t_float4& b, float t)
        {
            t_float4 r;
            r._glmt_quat = glm::slerp(a._glmt_quat, b._glmt_quat, t);
            return r;
        }

        friend std::ostream& operator<<(std::ostream& out, t_float4& v);
    };

    std::ostream& operator<<(std::ostream& out, t_float4& v);

    template <typename T, typename... S>
    struct t_vector : public T
    {
        t_vector(S... x)
            : T(x...)
        {
        }
        t_vector() {}
        t_vector(const void* v)
        {
            memcpy(this, v, sizeof(T));
        }

        t_vector(const T& v)
        {
            T::_glmt_vector = v._glmt_vector;
        }

        // Comparision operators
        bool operator==(const t_vector<T, S...>& v) const
        {
            return memcmp(T::v, v.v, sizeof(v.v)) == 0;
        }

        bool operator!=(const t_vector<T, S...>& v) const
        {
            return !(*this == v);
        }

        // Assignment operators
        t_vector<T, S...>& operator=(const t_vector<T, S...>& v)
        {
            T::_glmt_vector = v._glmt_vector;
            return *this;
        }
        t_vector<T, S...>& operator+=(const t_vector<T, S...>& v)
        {
            T::_glmt_vector += v._glmt_vector;
            return *this;
        }
        t_vector<T, S...>& operator-=(const t_vector<T, S...>& v)
        {
            T::_glmt_vector -= v._glmt_vector;
            return *this;
        }
        t_vector<T, S...>& operator*=(const t_vector<T, S...>& v)
        {
            T::_glmt_vector *= v._glmt_vector;
            return *this;
        }
        t_vector<T, S...>& operator*=(float s)
        {
            T::_glmt_vector *= s;
            return *this;
        }
        t_vector<T, S...>& operator/=(float s)
        {
            T::_glmt_vector *= s;
            return *this;
        }

        // Urnary operators
        t_vector<T, S...> operator+() const { return *this; }
        //t_vector<T, S...> operator- () const { return t_vector<T, S...>(-T::x, -T::y); }

        //TODO: float = S ;)
        float length() const { return glm::length(T::_glmt_vector); }
        float lengthSquared() const { return glm::length2(T::_glmt_vector); }
        float dot(const t_vector<T, S...>& v) const { return glm::dot(T::_glmt_vector, v._glmt_vector); }
        t_vector<T, S...> cross(const t_vector<T, S...>& v) const
        {
            t_vector<T, S...> r;
            r._glmt_vector = glm::cross(T::_glmt_vector, v._glmt_vector);

            return r;
        }
        //t_vector<T, S...> cross(const t_vector<T, S...>& v) { T::_glmt_vector = glm::cross(T::_glmt_vector, v._glmt_vector); return *this; }

        t_vector<T, S...> normalize()
        {
            T::_glmt_vector = glm::normalize(T::_glmt_vector);
            return *this;
        }

        // Static functions
        static float distance(const t_vector<T, S...>& v1, const t_vector<T, S...>& v2) { return (v2 - v1).length(); }
        static float distanceSquared(const t_vector<T, S...>& v1, const t_vector<T, S...>& v2) { return (v2 - v1).lengthSquared(); };
        static t_vector<T, S...> cross(const t_vector<T, S...>& v1, const t_vector<T, S...>& v2)
        {
            t_vector<T, S...> r;
            r._glmt_vector = glm::cross(v1._glmt_vector, v2._glmt_vector);
            return r;
        }

        static t_vector<T, S...> lerp(const t_vector<T, S...>& v1, const t_vector<T, S...>& v2, float p)
        {
            return (1.0f - p) * v1 + p * v2;
        }
    };

    template <typename T, typename... S>
    t_vector<T, S...> operator+(const t_vector<T, S...>& v1, const t_vector<T, S...>& v2)
    {
        t_vector<T, S...> r;
        r._glmt_vector = v1._glmt_vector + v2._glmt_vector;
        return r;
    }

    template <typename T, typename... S>
    t_vector<T, S...> operator-(const t_vector<T, S...>& v1, const t_vector<T, S...>& v2)
    {
        t_vector<T, S...> r;
        r._glmt_vector = v1._glmt_vector - v2._glmt_vector;
        return r;
    }

    template <typename T, typename... S>
    t_vector<T, S...> operator*(const t_vector<T, S...>& v1, float s)
    {
        t_vector<T, S...> r;
        r._glmt_vector = v1._glmt_vector * s;
        return r;
    }

    template <typename T, typename... S>
    t_vector<T, S...> operator*(float s, const t_vector<T, S...>& v2)
    {
        t_vector<T, S...> r;
        r._glmt_vector = s * v2._glmt_vector;
        return r;
    }

    template <typename T, typename... S>
    t_vector<T, S...> operator/(const t_vector<T, S...>& v1, float s)
    {
        t_vector<T, S...> r;
        r._glmt_vector = v1._glmt_vector / s;
        return r;
    }

    typedef t_vector<t_float2, float, float> float2;
    typedef t_vector<t_float3, float, float, float> float3;
    typedef t_vector<t_float4, float, float, float, float> float4;

    //------------------------------------------------------------------------------
    // 4x4 Matrix (assumes right-handed cooordinates)
    struct Matrix
    {
        Matrix() {}
        Matrix(const float* pm)
        {
            memcpy(m, pm, sizeof(m));
        }

        Matrix(const glm::mat4x4& m)
        {
            _glmMatrix = m;
        }

        Matrix(float m00, float m01, float m02, float m03,
               float m10, float m11, float m12, float m13,
               float m20, float m21, float m22, float m23,
               float m30, float m31, float m32, float m33)
        {
            m[0][0] = m00;
            m[0][1] = m01;
            m[0][2] = m02;
            m[0][3] = m03;

            m[1][0] = m10;
            m[1][1] = m11;
            m[1][2] = m12;
            m[1][3] = m13;

            m[2][0] = m20;
            m[2][1] = m21;
            m[2][2] = m22;
            m[2][3] = m23;

            m[3][0] = m30;
            m[3][1] = m31;
            m[3][2] = m32;
            m[3][3] = m33;
        }

        // Comparision operators
        bool operator==(const Matrix& M) const { return _glmMatrix == M._glmMatrix; }
        bool operator!=(const Matrix& M) const { return _glmMatrix != M._glmMatrix; }
        // Assignment operators
        Matrix& operator=(const Matrix& M)
        {
            memcpy(this, &M, sizeof(float) * 16);
            return *this;
        }
        Matrix& operator+=(const Matrix& M)
        {
            _glmMatrix += M._glmMatrix;
            return *this;
        }
        Matrix& operator-=(const Matrix& M)
        {
            _glmMatrix -= M._glmMatrix;
            return *this;
        }
        Matrix& operator*=(const Matrix& M)
        {
            _glmMatrix *= M._glmMatrix;
            return *this;
        }
        Matrix& operator*=(float s)
        {
            _glmMatrix *= s;
            return *this;
        }
        Matrix& operator/=(float s)
        {
            _glmMatrix *= s;
            return *this;
        }

        // Properties
        float3 Up() const { return float3(_21, _22, _23); }
        void Up(const float3& v)
        {
            _21 = v.x;
            _22 = v.y;
            _23 = v.z;
        }

        float3 Down() const { return float3(-_21, -_22, -_23); }
        void Down(const float3& v)
        {
            _21 = -v.x;
            _22 = -v.y;
            _23 = -v.z;
        }

        float3 Right() const { return float3(_11, _12, _13); }
        void Right(const float3& v)
        {
            _11 = v.x;
            _12 = v.y;
            _13 = v.z;
        }

        float3 Left() const { return float3(-_11, -_12, -_13); }
        void Left(const float3& v)
        {
            _11 = -v.x;
            _12 = -v.y;
            _13 = -v.z;
        }

        float3 Forward() const { return float3(-_31, -_32, -_33); }
        void Forward(const float3& v)
        {
            _31 = -v.x;
            _32 = -v.y;
            _33 = -v.z;
        }

        float3 Backward() const { return float3(_31, _32, _33); }
        void Backward(const float3& v)
        {
            _31 = v.x;
            _32 = v.y;
            _33 = v.z;
        }

        float3 Translation() const { return float3(_41, _42, _43); }
        float3 TranslationT() const { return float3(_14, _24, _34); }
        void Translation(const float3& v)
        {
            _41 = v.x;
            _42 = v.y;
            _43 = v.z;
        }

        Matrix Rotation()
        {
            Matrix m = *this;
            m.Translation(Math::float3(0, 0, 0));
            return m;
        }

        Matrix Transpose() const { return glm::transpose(_glmMatrix); }
        Matrix Invert() const { return glm::inverse(_glmMatrix); }
        Math::float3 Rotate(const Math::float3& v) const
        {
            Math::float3 tmp = (*this) * v;
            tmp -= Translation();

            return tmp;
        }

        float Determinant() const { return glm::determinant(_glmMatrix); }
        static Matrix CreateIdentity() { return Matrix(glm::mat4(1.0)); }
        static Matrix CreateTranslation(const float3& position) { return Matrix(glm::translate(glm::mat4x4(), position._glmt_vector)); }
        static Matrix CreateTranslation(float x, float y, float z) { return Matrix(glm::translate(glm::mat4x4(), glm::vec3(x, y, z))); }
        static Matrix CreateScale(const float3& scales) { return Matrix(glm::scale(glm::mat4x4(), scales._glmt_vector)); }
        static Matrix CreateScale(float xs, float ys, float zs) { return Matrix(glm::scale(glm::mat4x4(), glm::vec3(xs, ys, zs))); }
        static Matrix CreateScale(float scale) { return Matrix(glm::scale(glm::mat4x4(), glm::vec3(scale, scale, scale))); }
        static Matrix CreateRotationX(float radians) { return Matrix(glm::rotate(glm::mat4x4(), radians, glm::vec3(1, 0, 0))); }
        static Matrix CreateRotationY(float radians) { return Matrix(glm::rotate(glm::mat4x4(), radians, glm::vec3(0, 1, 0))); }
        static Matrix CreateRotationZ(float radians) { return Matrix(glm::rotate(glm::mat4x4(), radians, glm::vec3(0, 0, 1))); }
        static Matrix CreateFromAxisAngle(const float3& axis, float angle) { return Matrix(glm::rotate(glm::mat4x4(), angle, axis._glmt_vector)); }
        static Matrix CreatePerspectiveGL(float fov_deg, float width, float height, float nearPlane, float farPlane)
        {
            return Matrix(glm::perspectiveFovLH(fov_deg, width, height, nearPlane, farPlane));
        }

        static Matrix CreatePerspectiveDX(float fov_deg, float width, float height, float nearPlane, float farPlane)
        {
            return Matrix(glm::perspectiveFovLH(fov_deg, width, height, nearPlane, farPlane));

            const float fov_rad = glm::radians<float>(fov_deg);
            const float aspectRatio = width / height;
            const float yScale = 1 / tan(fov_rad / 2.0f);
            const float xScale = yScale / aspectRatio;

            glm::mat4x4 ret(0.0f);

            ret[0][0] = xScale;
            ret[1][1] = yScale;
            ret[2][2] = farPlane / (nearPlane - farPlane);
            ret[2][3] = -1;
            ret[3][2] = nearPlane * farPlane / (nearPlane - farPlane);

            return Matrix(ret);
        }

        static Matrix CreateOrthographic(float left, float right, float bottom, float top) { return Matrix(glm::ortho(left, right, bottom, top)); }
        static Matrix CreateLookAt(const float3& position, const float3& target, const float3& up) { return Matrix(glm::lookAtLH(position._glmt_vector, target._glmt_vector, up._glmt_vector)); }
        static Matrix CreateView(const float3& position, float yaw, float pitch)
        {
            /*Matrix mYaw = CreateRotationY(yaw);

			float3 fwd = float3(1.0f, 0.0f, 0.0f);
			fwd = mYaw * fwd;

			float3 side = fwd.cross(float3(0.0f,1.0f,0.0f));
			fwd = CreateFromAxisAngle(side, pitch) * fwd;

			return CreateLookAt(position, position + fwd, float3(0.0f, 1.0f, 0.0f));*/

            glm::vec3 direction(
                glm::cos(pitch) * glm::sin(yaw),
                glm::sin(pitch),
                glm::cos(pitch) * glm::cos(yaw));

            // Right vector
            glm::vec3 right = glm::vec3(
                glm::sin(yaw - 3.14f / 2.0f),
                0,
                glm::cos(yaw - 3.14f / 2.0f));

            glm::vec3 up = glm::cross(right, direction);

            return CreateLookAt(position,
                                position + float3(direction.x, direction.y, direction.z),
                                float3(up.x, up.y, up.z));
        }

        // TODO: Implement this
        static Matrix CreateFromQuaternion(const float4& quat)
        {
            Matrix m = Matrix::CreateIdentity();

            m.v[0].x = quat.w * quat.w + quat.x * quat.x - quat.y * quat.y - quat.z * quat.z;
            m.v[0].y = 2.0f * (quat.x * quat.y + quat.w * quat.z);
            m.v[0].z = 2.0f * (quat.x * quat.z - quat.w * quat.y);
            m.v[1].x = 2.0f * (quat.x * quat.y - quat.w * quat.z);
            m.v[1].y = quat.w * quat.w - quat.x * quat.x + quat.y * quat.y - quat.z * quat.z;
            m.v[1].z = 2.0f * (quat.y * quat.z + quat.w * quat.x);
            m.v[2].x = 2.0f * (quat.x * quat.z + quat.w * quat.y);
            m.v[2].y = 2.0f * (quat.y * quat.z - quat.w * quat.x);
            m.v[2].z = quat.w * quat.w - quat.x * quat.x - quat.y * quat.y + quat.z * quat.z;

            return m.Transpose();
            //return glm::mat4_cast(glm::quat(quat.x, quat.y, quat.z, quat.w));
        }

        union {
            struct
            {
                float _11, _12, _13, _14;
                float _21, _22, _23, _24;
                float _31, _32, _33, _34;
                float _41, _42, _43, _44;
            };
            float m[4][4];
            float4 v[4];
            float mv[16];
            glm::mat4x4 _glmMatrix;
        };

        std::string toString()
        {
            std::string out;
            out = "[";
            for (size_t i = 0; i < 16; i++)
            {
                out += std::to_string(mv[i]);

                // Only add "," when not at the last value
                if (i != 15)
                    out += ", ";
            }
            out += "]";

            return out;
        }

        friend std::ostream& operator<<(std::ostream& out, Matrix& v);
        friend float4 operator*(const Matrix& m, const float4& v);
        friend float3 operator*(const Matrix& m, const float3& v);
        friend Matrix operator*(const Matrix& M1, const Matrix& M2);
    };

    inline float4 operator*(const Matrix& m, const float4& v)
    {
        float4 r;
        r._glmt_vector = m._glmMatrix * v._glmt_vector;
        return r;
    }

    /**
	 * @brief Shortcut to full mat4-vec3 transformation
	 */
    inline float3 operator*(const Matrix& m, const float3& v)
    {
        float4 r;
        r._glmt_vector = m._glmMatrix * float4(v.x, v.y, v.z, 1.0f)._glmt_vector;
        return float3(r.x, r.y, r.z);
    }

    std::ostream& operator<<(std::ostream& out, Matrix& m);

    inline Matrix operator*(const Matrix& M1, const Matrix& M2)
    {
        return M1._glmMatrix * M2._glmMatrix;
    }

    /** 
	* @brief Compute barycentric coordinates (u, v, w) for
	*		  point p with respect to triangle (a, b, c)
	*		  (Transcribed from Christer Ericson's Real-Time Collision Detection)
	*/

    static void barycentric(const float3& p, const float3& a, const float3& b, const float3& c, float& u, float& v, float& w)
    {
        float3 v0 = b - a, v1 = c - a, v2 = p - a;
        float d00 = v0.dot(v0);
        float d01 = v0.dot(v1);
        float d11 = v1.dot(v1);
        float d20 = v2.dot(v0);
        float d21 = v2.dot(v1);
        float denom = d00 * d11 - d01 * d01;
        v = (d11 * d20 - d01 * d21) / denom;
        w = (d00 * d21 - d01 * d20) / denom;
        u = 1.0f - v - w;
    }

    /**
     * @brief if v compares less than hi, returns the larger of v and lo, otherwise returns the smaller of v and hi
     */
    template <class T>
    T clamp(T v, T lo, T hi)
    {
        assert(lo <= hi);
        return (v < lo) ? lo : (hi < v) ? hi : v;
    }
}
