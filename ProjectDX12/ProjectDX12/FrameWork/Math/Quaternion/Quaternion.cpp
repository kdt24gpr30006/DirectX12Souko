//
#include <math.h>
#include "Quaternion.h"
#include "../Matrix/Matrix.h"
#include "../Matrix3x3/Matrix3x3.h"
#include "../Vector3/Vector3.h"

namespace Math
{
	const Quaternion Quaternion::Identity = { 0.0f, 0.0f, 0.0f, 1.0f };

	Matrix Quaternion::ToMatrix()const
	{
		const Matrix3x3 mat = ToMatrix3x3();
		Matrix m = {};
		m._11 = mat._11;
		m._12 = mat._12;
		m._13 = mat._13;

		m._21 = mat._21;
		m._22 = mat._22;
		m._23 = mat._23;

		m._31 = mat._31;
		m._32 = mat._32;
		m._33 = mat._33;

		m._44 = 1.0f;
		return m;
	}

	Matrix3x3 Quaternion::ToMatrix3x3()const
	{
		const float Qx = x * x;
		const float Qy = y * y;
		const float Qz = z * z;
		const float Qw = w * w;

		Matrix3x3 m = {};

		m._11 = Qx - Qy - Qz + Qw;
		m._12 = 2 * (x * y + z * w);
		m._13 = 2 * (x * z - y * w);

		m._21 = 2 * (x * y - z * w);
		m._22 = -Qx + Qy - Qz + Qw;
		m._23 = 2 * (y * z + x * w);

		m._31 = 2 * (x * z + y * w);
		m._32 = 2 * (y * z - x * w);
		m._33 = -Qx - Qy + Qz + Qw;

		return m;
	}

	Quaternion Quaternion::Multiply(const Quaternion& p, const Quaternion& q)
	{
		Quaternion qua = {};

		qua.x = (q.w * p.x) - (q.z * p.y) + (q.y * p.z) + (q.x * p.w);
		qua.y = (q.z * p.x) + (q.w * p.y) - (q.x * p.z) + (q.y * p.w);
		qua.z = -(q.y * p.x) + (q.x * p.y) + (q.w * p.z) + (q.z * p.w);
		qua.w = -(q.x * p.x) - (q.y * p.y) - (q.z * p.z) + (q.w * p.w);

		return qua;
	}

	Quaternion Quaternion::AngleAxis(float angle, const Vector3& axis)
	{
		const float a = angle * 0.5f;
		const float sin = sinf(a);

		Vector3 v = axis;
		v.Normalize();

		const Quaternion q =
		{
			v.x * sin,
			v.y * sin,
			v.z * sin,
			cosf(a)
		};
		return q;
	}

	Quaternion Quaternion::Lerp(const Quaternion& a, const Quaternion& b, float t)
	{
		const float u = 1.0f - t;
		Quaternion q = Quaternion::Identity;
		q.x = a.x * u + b.x * t;
		q.y = a.y * u + b.y * t;
		q.z = a.z * u + b.z * t;
		q.w = a.w * u + b.w * t;
		return q;
	}

	Quaternion Quaternion::Slerp(const Quaternion& a, const Quaternion& b, float t)
	{
		Quaternion p = a;
		Quaternion q = b;

		//	内積計算
		float dot = Dot(p, q);
		if (dot < 0.0f)
		{
			q.x = -q.x;
			q.y = -q.y;
			q.z = -q.z;
			q.w = -q.w;
			dot = -dot;
		}
		float u = 0.0f, v=0.0f;
		//	密接具合チェック
		if (dot > 0.9f)
		{
			return Lerp(p, q, t);
		}
		else
		{
			float sin = sqrtf(1.0f - dot * dot);
			float angle = atan2f(sin, dot);

			float value = 1.0f / sin;
			u = sinf((1.0f - t)* angle) * value;
			v = sinf(t* angle) * value;
		}
		const Quaternion ret =
		{
			q.x * u + p.x * v,
			q.y * u + p.y * v,
			q.z * u + p.z * v,
			q.w * u + p.w * v
		};
		return ret;
	}

}
