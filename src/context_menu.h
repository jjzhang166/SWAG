#ifndef swag_context_menu_h
#define swag_context_menu_h

#include "container.h"
#include <string>

class Menu;

class Context_menu: public Container
{
public:
	Context_menu();
	virtual Widget* Clone() const;
	void Set_context(Widget* widget);
	Widget* Get_context() const;
	virtual bool Add_child(Widget* c);
	virtual void Remove_child(Widget* c);
	virtual Widgets Get_children() const;
	virtual void To_yaml(YAML::Emitter& out) const;
private:
	Widget* context;
	//Menu is a separate class that holds the menu structure and info for rendering
	//Since context menu is only a container for the context.
	//The menu class can also be used for popping up from a Menu_bar
	Menu* menu;
};

#endif
