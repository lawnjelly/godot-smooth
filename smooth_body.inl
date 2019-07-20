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

// where smoothclass is defined as the class
// and smoothnode is defined as the node

//#define SMOOTHVERBOSE

#define SMOOTH_STRINGIFY(x) #x
#define SMOOTH_TOSTRING(x) SMOOTH_STRINGIFY(x)

void SMOOTHCLASS::SetProcessing()
{
	if (Engine::get_singleton()->is_editor_hint())
		return;

	bool bEnable = TestFlags(SF_ENABLED);
	if (TestFlags(SF_INVISIBLE))
		bEnable = false;

	set_process_internal(bEnable);
	set_physics_process_internal(bEnable);
}


void SMOOTHCLASS::set_enabled(bool p_enabled)
{
	if (TestFlags(SF_ENABLED) == p_enabled)
		return;

	ChangeFlags(SF_ENABLED, p_enabled);

	SetProcessing ();
}

bool SMOOTHCLASS::is_enabled() const
{
	return TestFlags(SF_ENABLED);
}

void SMOOTHCLASS::set_interpolate_translation(bool bTranslate)
{
	ChangeFlags(SF_TRANSLATE, bTranslate);
}

bool SMOOTHCLASS::get_interpolate_translation() const
{
	return TestFlags(SF_TRANSLATE);
}


void SMOOTHCLASS::set_interpolate_rotation(bool bRotate)
{
	ChangeFlags(SF_ROTATE, bRotate);
}

bool SMOOTHCLASS::get_interpolate_rotation() const
{
	return TestFlags(SF_ROTATE);
}

void SMOOTHCLASS::set_interpolate_scale(bool bScale)
{
	ChangeFlags(SF_SCALE, bScale);
}

bool SMOOTHCLASS::get_interpolate_scale() const
{
	return TestFlags(SF_SCALE);
}


void SMOOTHCLASS::set_input_mode(eMode p_mode)
{
	if (p_mode == MODE_GLOBAL)
		SetFlags(SF_GLOBAL_IN);
	else
		ClearFlags(SF_GLOBAL_IN);
}

SMOOTHCLASS::eMode SMOOTHCLASS::get_input_mode() const
{
	if (TestFlags(SF_GLOBAL_IN))
		return MODE_GLOBAL;

	return MODE_LOCAL;
}

void SMOOTHCLASS::set_output_mode(eMode p_mode)
{
	if (p_mode == MODE_GLOBAL)
		SetFlags(SF_GLOBAL_OUT);
	else
		ClearFlags(SF_GLOBAL_OUT);
}

SMOOTHCLASS::eMode SMOOTHCLASS::get_output_mode() const
{
	if (TestFlags(SF_GLOBAL_OUT))
		return MODE_GLOBAL;

	return MODE_LOCAL;
}


void SMOOTHCLASS::RemoveTarget()
{
	NodePath pathnull;
	set_target_path(pathnull);
}

void SMOOTHCLASS::smooth_print_line(Variant sz)
{
#ifdef SMOOTHVERBOSE
		print_line(sz);
#endif
}


void SMOOTHCLASS::set_target(const Object *p_target)
{
	// handle null
	if (p_target == NULL)
	{
		smooth_print_line("SCRIPT set_Target NULL");
		RemoveTarget();
		return;
	}
	smooth_print_line("SCRIPT set_Target");

	// is it a SMOOTHNODE?
	const SMOOTHNODE * pSMOOTHNODE = Object::cast_to<SMOOTHNODE>(p_target);

	if (!pSMOOTHNODE)
	{
		WARN_PRINT("Target must be a "  SMOOTH_TOSTRING(SMOOTHNODE) ".");
		RemoveTarget();
		return;
	}
	smooth_print_line("Target successfully cast to "  SMOOTH_TOSTRING(SMOOTHNODE) ".");

	NodePath path = get_path_to(pSMOOTHNODE);
	set_target_path(path);
}

void SMOOTHCLASS::_set_target(const Object *p_target)
{
//	m_refTarget.set_obj(pTarget);
	if (p_target)
		smooth_print_line("\t_set_Target");
	else
		smooth_print_line("\t_set_Target NULL");

	m_ID_target = 0;

	if (p_target)
	{
		// check is SMOOTHNODE
		const SMOOTHNODE *pSMOOTHNODE = Object::cast_to<SMOOTHNODE>(p_target);

		if (pSMOOTHNODE)
		{
			m_ID_target = pSMOOTHNODE->get_instance_id();
			smooth_print_line("\t\tTarget was SMOOTHNODE ID " + itos(m_ID_target));
		}
		else
		{
			// should never get here?
			WARN_PRINT(SMOOTH_TOSTRING(SMOOTHCLASS) " target must be a "  SMOOTH_TOSTRING(SMOOTHNODE) ".");
		}
	}

}

void SMOOTHCLASS::ResolveTargetPath()
{
	smooth_print_line("resolve_Target_path " + m_path_target);

	if (has_node(m_path_target))
	{
		smooth_print_line("has_node");
		SMOOTHNODE * pNode = Object::cast_to<SMOOTHNODE>(get_node(m_path_target));
		if (pNode)
		{
			smooth_print_line("node_was " SMOOTH_TOSTRING(SMOOTHNODE));
			_set_target(pNode);
			return;
		}
		else
		{
			WARN_PRINT(SMOOTH_TOSTRING(SMOOTHCLASS) " target must be a "  SMOOTH_TOSTRING(SMOOTHNODE) ".");
		}
	}

	// default to setting to null
	_set_target(NULL);
}

void SMOOTHCLASS::set_target_path(const NodePath &p_path)
{
	smooth_print_line("set_Target_path " + p_path);
	m_path_target = p_path;
	ResolveTargetPath();
}

NodePath SMOOTHCLASS::get_target_path() const
{
	return m_path_target;
}

SMOOTHNODE * SMOOTHCLASS::GetTarget() const
{
	if (m_ID_target == 0)
		return 0;

	Object *obj = ObjectDB::get_instance(m_ID_target);
	return (SMOOTHNODE *) obj;
}

void SMOOTHCLASS::FixedUpdate()
{
	// has more than one physics tick occurred before a frame? if so
	// refresh the transform to the last physics tick. In most cases this will not occur,
	// except with tick rates higher than frame rate
	if (TestFlags(SF_DIRTY))
	{
		SMOOTHNODE * pTarget = GetTarget();
		if (pTarget)
		{
			RefreshTransform(pTarget);
		}
	}

	// refresh the transform on the next frame
	// because the Target transform may not be updated until AFTER the fixed update!
	// (this gets around an order of operations issue)
	SetFlags(SF_DIRTY);
}



void SMOOTHCLASS::_notification(int p_what) {

	switch (p_what) {
	case NOTIFICATION_ENTER_TREE: {
			bool bVisible = is_visible_in_tree();
			ChangeFlags(SF_INVISIBLE, bVisible == false);
			SetProcessing();

			// we can't translate string name of Target to a node until we are in the tree
			ResolveTargetPath();
		} break;
	case NOTIFICATION_INTERNAL_PHYSICS_PROCESS: {
			FixedUpdate();
		} break;
	case NOTIFICATION_INTERNAL_PROCESS: {
			FrameUpdate();
		} break;
	case NOTIFICATION_VISIBILITY_CHANGED: {
			bool bVisible = is_visible_in_tree();
			ChangeFlags(SF_INVISIBLE, bVisible == false);
			SetProcessing();
//			if (bVisible)
//				print_line("now visible");
//			else
//				print_line("now hidden");
		} break;
	}
}



void SMOOTHCLASS::_bind_methods() {

	BIND_ENUM_CONSTANT(MODE_LOCAL);
	BIND_ENUM_CONSTANT(MODE_GLOBAL);


	ClassDB::bind_method(D_METHOD("teleport"), &SMOOTHCLASS::teleport);

	ClassDB::bind_method(D_METHOD("set_enabled"), &SMOOTHCLASS::set_enabled);
	ClassDB::bind_method(D_METHOD("is_enabled"), &SMOOTHCLASS::is_enabled);
	ClassDB::bind_method(D_METHOD("set_smooth_translate"), &SMOOTHCLASS::set_interpolate_translation);
	ClassDB::bind_method(D_METHOD("get_smooth_translate"), &SMOOTHCLASS::get_interpolate_translation);
	ClassDB::bind_method(D_METHOD("set_smooth_rotate"), &SMOOTHCLASS::set_interpolate_rotation);
	ClassDB::bind_method(D_METHOD("get_smooth_rotate"), &SMOOTHCLASS::get_interpolate_rotation);
	ClassDB::bind_method(D_METHOD("set_smooth_scale"), &SMOOTHCLASS::set_interpolate_scale);
	ClassDB::bind_method(D_METHOD("get_smooth_scale"), &SMOOTHCLASS::get_interpolate_scale);

	ClassDB::bind_method(D_METHOD("set_input_mode", "mode"), &SMOOTHCLASS::set_input_mode);
	ClassDB::bind_method(D_METHOD("get_input_mode"), &SMOOTHCLASS::get_input_mode);
	ClassDB::bind_method(D_METHOD("set_output_mode", "mode"), &SMOOTHCLASS::set_output_mode);
	ClassDB::bind_method(D_METHOD("get_output_mode"), &SMOOTHCLASS::get_output_mode);

	ClassDB::bind_method(D_METHOD("set_target", "target"), &SMOOTHCLASS::set_target);
	ClassDB::bind_method(D_METHOD("set_target_path", "path"), &SMOOTHCLASS::set_target_path);
	ClassDB::bind_method(D_METHOD("get_target_path"), &SMOOTHCLASS::get_target_path);


	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enabled"), "set_enabled", "is_enabled");

	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "target"), "set_target_path", "get_target_path");


	ADD_GROUP("Components", "");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "smooth_translate"), "set_smooth_translate", "get_smooth_translate");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "smooth_rotate"), "set_smooth_rotate", "get_smooth_rotate");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "smooth_scale"), "set_smooth_scale", "get_smooth_scale");
	ADD_GROUP("Coords", "");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "input", PROPERTY_HINT_ENUM, "Local,Global"), "set_input_mode", "get_input_mode");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "output", PROPERTY_HINT_ENUM, "Local,Global"), "set_output_mode", "get_output_mode");
//	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "lerp"), "set_lerp", "get_lerp");
//}
#undef SMOOTH_STRINGIFY
#undef SMOOTH_TOSTRING
