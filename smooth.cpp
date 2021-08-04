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

#include "smooth.h"
#include "core/engine.h"

#define SMOOTHCLASS Smooth
#define SMOOTHNODE Spatial
#include "smooth_body.inl"

// finish the bind with custom stuff
BIND_ENUM_CONSTANT(METHOD_SLERP);
BIND_ENUM_CONSTANT(METHOD_LERP);
ClassDB::bind_method(D_METHOD("set_method", "method"), &SMOOTHCLASS::set_method);
ClassDB::bind_method(D_METHOD("get_method"), &SMOOTHCLASS::get_method);

ADD_GROUP("Misc", "");
ADD_PROPERTY(PropertyInfo(Variant::INT, "method", PROPERTY_HINT_ENUM, "Slerp,Lerp"), "set_method", "get_method");
}

#undef SMOOTHCLASS
#undef SMOOTHNODE

Smooth::Smooth() {
	m_Flags = 0;
	SetFlags(SF_ENABLED | SF_TRANSLATE | SF_ROTATE | SF_LERP);
	set_process_priority(100);
	Engine::get_singleton()->set_physics_jitter_fix(0.0);
}

void Smooth::set_method(eMethod p_method) {
	ChangeFlags(SF_LERP, p_method == METHOD_LERP);
}

Smooth::eMethod Smooth::get_method() const {
	if (TestFlags(SF_LERP))
		return METHOD_LERP;

	return METHOD_SLERP;
}

void Smooth::teleport() {
	Spatial *pTarget = GetTarget();
	if (!pTarget)
		return;

	// refresh all components during teleport
	int temp_flags = m_Flags;
	SetFlags(SF_TRANSLATE | SF_ROTATE | SF_SCALE | SF_LERP);

	RefreshTransform(pTarget);

	// set previous equal to current
	m_Prev = m_Curr;
	m_ptTranslateDiff.zero();

	// call frame upate to make sure all components of the node are set
	FrameUpdate();

	// restore flags
	m_Flags = temp_flags;
}

void Smooth::RefreshTransform(Spatial *pTarget) {
	ClearFlags(SF_DIRTY);

	// keep the data flowing...
	// translate..
	m_Prev.m_Transform.origin = m_Curr.m_Transform.origin;

	// global or local?
	bool bGlobal = TestFlags(SF_GLOBAL_IN);

	// new transform for this tick
	Transform trans;
	if (bGlobal)
		trans = pTarget->get_global_transform();
	else
		trans = pTarget->get_transform();

	if (TestFlags(SF_TRANSLATE)) {
		m_Curr.m_Transform.origin = trans.origin;
		m_ptTranslateDiff = m_Curr.m_Transform.origin - m_Prev.m_Transform.origin;
	}

	// lerp? keep the basis
	if (TestFlags(SF_LERP)) {
		m_Prev.m_Transform.basis = m_Curr.m_Transform.basis;
		m_Curr.m_Transform.basis = trans.basis;
	} else {
		if (TestFlags(SF_ROTATE)) {
			m_Prev.m_qtRotate = m_Curr.m_qtRotate;
			m_Curr.m_qtRotate = trans.basis.get_rotation_quat();
		}

		if (TestFlags(SF_SCALE)) {
			m_Prev.m_ptScale = m_Curr.m_ptScale;
			m_Curr.m_ptScale = trans.basis.get_scale();
		}

	} // if not lerp
}

void Smooth::FrameUpdate() {
	Spatial *pTarget = GetTarget();
	if (!pTarget)
		return;

	if (TestFlags(SF_DIRTY))
		RefreshTransform(pTarget);

	// interpolation fraction
	float f = Engine::get_singleton()->get_physics_interpolation_fraction();

	Vector3 ptNew = m_Prev.m_Transform.origin + (m_ptTranslateDiff * f);

	// global or local?
	bool bGlobal = TestFlags(SF_GLOBAL_OUT);

	// simplified, only using translate .. useful for e.g. moving platforms that don't rotate
	// NOTE THIS IMPLIES LOCAL as global flag not set...
	if (m_Flags == (SF_ENABLED | SF_TRANSLATE)) {
		set_translation(ptNew);
		return;
	}

	// send as a transform, the whole kabunga
	Transform trans;
	trans.origin = ptNew;

	// lerping
	if (TestFlags(SF_LERP)) {
		//trans.basis = m_Prev.m_Basis.slerp(m_Curr.m_Basis, f);
		LerpBasis(m_Prev.m_Transform.basis, m_Curr.m_Transform.basis, trans.basis, f);
	} else {
		// slerping
		Quat qtRot = m_Prev.m_qtRotate.slerp(m_Curr.m_qtRotate, f);
		trans.basis.set_quat(qtRot);

		if (TestFlags(SF_SCALE)) {
			Vector3 ptScale = ((m_Curr.m_ptScale - m_Prev.m_ptScale) * f) + m_Prev.m_ptScale;
			trans.basis.scale(ptScale);
		}
	}

	if (bGlobal)
		set_global_transform(trans);
	else
		set_transform(trans);
}

// Directly lerping the basis vectors is cheaper than doing a slerp and dealing with scale properly
// Could look bad with large changes between ticks, but with reasonable tick rate usually looks acceptable.
void Smooth::LerpBasis(const Basis &from, const Basis &to, Basis &res, float f) const {
	res = from;

	for (int n = 0; n < 3; n++)
		res.elements[n] = from.elements[n].linear_interpolate(to.elements[n], f);
}

//bool Smooth::FindVisibility() const
//{
//	const Spatial *s = this;

//	int count = 0;
//	while (s) {

//		if (!s->data.visible)
//		{
//			print_line(itos(count++) + " hidden");
//			return false;
//		}
//		else
//		{
//			print_line(itos(count++) + " visible");
//		}
//		s = s->data.parent;
//	}

//	return true;
//}
