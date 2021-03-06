#ifndef editor_controller_h
#define editor_controller_h

#include "controller.h"

class Layout_controller;

class Editor_controller: public Controller {
public:
	Editor_controller();
	virtual bool Load(Skin& skin);
	void Set_layout_controller(Layout_controller& lc);
	virtual Widget* Get_root();
	virtual void Handle_event(const Ustring& event_handle, const Event& event);
	virtual void Synchronize_values();
	void Set_layout_display(ALLEGRO_DISPLAY *display);
	virtual void Update();
private:
	Layout_controller* layout_controller;
	ALLEGRO_DISPLAY *layout_display;
	typedef std::map<std::string, Controller*> Attribute_controllers;
	Attribute_controllers attribute_controllers;
	Event_queue gui_events;
	Widget* cut_widget;
	Tree* cut_tree;
};

#endif
