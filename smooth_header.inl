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

public:
	enum eMode
	{
		MODE_LOCAL,
		MODE_GLOBAL,
	};
private:
	enum eSmoothFlags
	{
		SF_ENABLED = 1,
		SF_DIRTY = 2,
		SF_TRANSLATE = 4,
		SF_ROTATE = 8,
		SF_SCALE = 16,
		SF_GLOBAL_IN = 32,
		SF_GLOBAL_OUT = 64,
		SF_LERP = 128,
		SF_INVISIBLE = 256,
	};


	STransform m_Curr;
	STransform m_Prev;


	// defined by eSmoothFlags
	int m_Flags;
//	eMode m_Mode;

	//WeakRef m_refTarget;
	ObjectID m_ID_target;
	NodePath m_path_target;

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	void set_input_mode(eMode p_mode);
	eMode get_input_mode() const;
	void set_output_mode(eMode p_mode);
	eMode get_output_mode() const;

	void set_enabled(bool p_enabled);
	bool is_enabled() const;

	void set_interpolate_translation(bool bTranslate);
	bool get_interpolate_translation() const;

	void set_interpolate_rotation(bool bRotate);
	bool get_interpolate_rotation() const;

	void set_interpolate_scale(bool bScale);
	bool get_interpolate_scale() const;


	void set_target(const Object *p_target);
	void set_target_path(const NodePath &p_path);
	NodePath get_target_path() const;

	void teleport();


private:
	void _set_target(const Object *p_target);
	void RemoveTarget();
	void FixedUpdate();
	void FrameUpdate();
	void RefreshTransform(SMOOTHNODE * pTarget);
	SMOOTHNODE * GetTarget() const;
	void SetProcessing();

	void ChangeFlags(int f, bool bSet) {if (bSet) {SetFlags(f);} else {ClearFlags(f);}}
	void SetFlags(int f) {m_Flags |= f;}
	void ClearFlags(int f) {m_Flags &= ~f;}
	bool TestFlags(int f) const {return (m_Flags & f) == f;}

	void ResolveTargetPath();
	//bool FindVisibility() const;
	void smooth_print_line(Variant sz);
