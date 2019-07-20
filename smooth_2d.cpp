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

#include "smooth_2d.h"
#include "core/engine.h"


#define SMOOTHCLASS Smooth2D
#define SMOOTHNODE Node2D
#include "smooth_body.inl"

// finish the bind with custom stuff
}

#undef SMOOTHCLASS
#undef SMOOTHNODE


////////////////////////////////

Smooth2D::Smooth2D()
{
	m_Flags = 0;
	SetFlags(SF_ENABLED | SF_TRANSLATE);
}


float Smooth2D::LerpAngle(float from, float to, float weight) const
{
    return from + ShortAngleDist(from, to) * weight;
}

float Smooth2D::ShortAngleDist(float from, float to) const
{
	float max_angle = (3.14159265358979323846264 * 2.0);
    float difference = fmod(to - from, max_angle);
    return fmod(2.0f * difference, max_angle) - difference;
}

void Smooth2D::RefreshTransform(Node2D * pTarget)
{
	//print_line("RefreshTransform");
	ClearFlags(SF_DIRTY);

	// keep the data flowing...
	m_Prev = m_Curr;

	// global or local?
	bool bGlobal = TestFlags(SF_GLOBAL_IN);

	if (bGlobal)
	{
		// new transform for this tick
		if (TestFlags(SF_TRANSLATE))
		{
			m_Curr.pos = pTarget->get_global_position();
			m_ptTranslateDiff = m_Curr.pos - m_Prev.pos;
		}


		if (TestFlags(SF_ROTATE))
			m_Curr.angle = pTarget->get_global_rotation();

		if (TestFlags(SF_SCALE))
			m_Curr.scale = pTarget->get_global_scale();
	}
	else
	{
		// new transform for this tick
		if (TestFlags(SF_TRANSLATE))
		{
			m_Curr.pos = pTarget->get_position();
			m_ptTranslateDiff = m_Curr.pos - m_Prev.pos;
		}


		if (TestFlags(SF_ROTATE))
			m_Curr.angle = pTarget->get_rotation();

		if (TestFlags(SF_SCALE))
			m_Curr.scale = pTarget->get_scale();
	}

}


void Smooth2D::FrameUpdate()
{
	//print_line("FrameUpdate");

	Node2D * pTarget = GetTarget();
	if (!pTarget)
	return;

	if (TestFlags(SF_DIRTY))
		RefreshTransform(pTarget);

	// interpolation fraction
	float f = Engine::get_singleton()->get_physics_interpolation_fraction();


	// global or local?
	bool bGlobal = TestFlags(SF_GLOBAL_OUT);

	if (bGlobal)
	{
		if (TestFlags(SF_TRANSLATE))
		{
			Point2 ptNew = m_Prev.pos + (m_ptTranslateDiff * f);
			set_global_position(ptNew);
		}

		if (TestFlags(SF_ROTATE))
		{
			// need angular interp
			float r = LerpAngle(m_Prev.angle, m_Curr.angle, f);
			set_global_rotation(r);
		}

		if (TestFlags(SF_SCALE))
		{
			Size2 s = m_Prev.scale + ((m_Curr.scale - m_Prev.scale) * f);
			set_global_scale(s);
		}
	}
	else
	{
		if (TestFlags(SF_TRANSLATE))
		{
			Point2 ptNew = m_Prev.pos + (m_ptTranslateDiff * f);
			set_position(ptNew);
		}

		if (TestFlags(SF_ROTATE))
		{
			// need angular interp
			float r = LerpAngle(m_Prev.angle, m_Curr.angle, f);
			set_rotation(r);
		}

		if (TestFlags(SF_SCALE))
		{
			Size2 s = m_Prev.scale + ((m_Curr.scale - m_Prev.scale) * f);
			set_scale(s);
		}
	}



}

void Smooth2D::teleport()
{
	Node2D * pTarget = GetTarget();
	if (!pTarget)
	return;

	// refresh all components during teleport
	int temp_flags = m_Flags;
	SetFlags(SF_TRANSLATE | SF_ROTATE | SF_SCALE | SF_LERP);

	RefreshTransform(pTarget);

	// set previous equal to current
	m_Prev = m_Curr;
	m_ptTranslateDiff.x = 0.0f;
	m_ptTranslateDiff.y = 0.0f;

	// call frame upate to make sure all components of the node are set
	FrameUpdate();

	// restore flags
	m_Flags = temp_flags;

}

//bool Smooth2D::FindVisibility() const
//{
//	return is_visible_in_tree();
//}

