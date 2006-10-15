/**
 * @file MFVector.h
 * @brief Describes the Mount Fuji Engine vector class.
 * @author Manu Evans
 * @defgroup MFQuaternion Quaternion math Class
 * @{
 */

#if !defined(_MFQUATERNION_H)
#define _MFQUATERNION_H

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

MFALIGN_BEGIN(16)
class MFQuaternion
{
public:
	struct
	{
		float x, y, z, w;
	};

	static const MFQuaternion identity;

	// general purpose quaternion operators
	bool operator==(const MFQuaternion &q) const;
	bool operator!=(const MFQuaternion &q) const;

	void SetIdentity();

	void Set(float x, float y, float z, float w);

	MFQuaternion& operator=(const MFQuaternion &q);
	MFQuaternion operator*(const MFQuaternion &q) const;

	MFQuaternion& Multiply(const MFQuaternion &q);
	MFQuaternion& Multiply(const MFQuaternion &q1, const MFQuaternion &q2);

	MFQuaternion& Conjugate(const MFQuaternion &q);
	MFQuaternion& Conjugate();

	operator float*();
	operator const float*() const;

	MFQuaternion& SLerp(const MFQuaternion &v, float t);

	const char * ToString() const;
}
MFALIGN_END(16);

MFQuaternion MakeQuat(float x, float y, float z, float w);

#include "MFQuaternion.inl"

// this is to make doxygen happy...
#if 0

/**
 * Quaternion class to perform various quaternion operations.
 * Quaternion class used throughout Fuji. Optimised for each platform.
 */
class MFQuaternion
{
public:
	struct
	{
		float x; /**< X imaginary component */
		float y; /**< Y imaginary component */
		float z; /**< Z imaginary component */
		float w; /**< W real component */
	};

	static const MFQuaternion identity;							/**< Constant IDENTITY vector { 0, 0, 0, 1 } */


	// general purpose vector operators

	bool operator==(const MFQuaternion &q) const;				/**< Comparison operator. */
	bool operator!=(const MFQuaternion &q) const;				/**< Not-equals operator. */

	void SetIdentity();											/**< Set quaternion to the identity quaternion. */

	void Set(float x, float y, float z = 0.0f, float w = 1.0f);	/**< Set each component of a quaternion. */

	MFVector& operator=(const MFVector &v);						/**< Equals operator. */
	MFQuaternion operator*(const MFQuaternion &q) const;		/**< Quaternion multiply operator. */

	MFQuaternion& Multiply(const MFQuaternion &q);				/**< Perform a quaternion multiply. */
	MFQuaternion& Multiply(const MFQuaternion &q1, const MFQuaternion &q2);	/**< Perform a quaternion multiply. */

	MFQuaternion& Conjugate(const MFQuaternion &q);				/**< Find the conjugate of a quaternion. */
	MFQuaternion& Conjugate();									/**< Calculate the quaternion conjugate. */

	operator float*();											/**< float pointer cast operator. */
	operator float*() const;									/**< const float pointer cast operator. */

	MFVector& SLerp(const MFQuaternion &q, float t);			/**< SLerp between 2 quaternions. */

	const char * ToString() const;								/**< Convert quaternion to a string. */
};

#endif // make doxygen happy

#endif // _MFQUATERNION_H

/** @} */
