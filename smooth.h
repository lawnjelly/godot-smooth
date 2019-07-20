//	Copyright (c) 2019 Lawnjelly

//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:

//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.

//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.

/* smooth.h */
#ifndef SMOOTH_H
#define SMOOTH_H

/**
	@author lawnjelly <lawnjelly@gmail.com>
*/


#include "scene/3d/spatial.h"


// Smooth node allows fixed timestep interpolation without having to write any code.
// It requires a proxy node (which is moved on physics tick), e.g. a rigid body or manually moved spatial..
// and instead of having MeshInstance as a child of this, you add Smooth node to another part of the scene graph,
// make the MeshInstance a child of the smooth node, then choose the proxy as the target for the smooth node.

// Note that in the special case of manually moving the proxy to a completely new location, you should call
// 'teleport' on the smooth node after setting the proxy node transform. This will ensure that the current AND
// previous transform records are reset, so it moves instantaneously.

class Smooth : public Spatial {
	GDCLASS(Smooth, Spatial);

// custom
private:
	class STransform
	{
	public:
		Transform m_Transform;
		Quat m_qtRotate;
		Vector3 m_ptScale;
	};

	Vector3 m_ptTranslateDiff;

public:
	enum eMethod
	{
		METHOD_SLERP,
		METHOD_LERP,
	};

#define SMOOTHNODE Spatial
#include "smooth_header.inl"
#undef SMOOTHNODE



// specific
public:
	Smooth();

	void set_method(eMethod p_method);
	eMethod get_method() const;
private:
	void LerpBasis(const Basis &from, const Basis &to, Basis &res, float f) const;
};

VARIANT_ENUM_CAST(Smooth::eMode);
VARIANT_ENUM_CAST(Smooth::eMethod);

#endif
