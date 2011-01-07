#ifndef swag_expander_h
#define swag_expander_h

#include "container.h"
#include <string>

class Expander: public Container
{
public:
	Expander();
	virtual Widget* Clone() const;
	bool Is_open() const;
	void Open();
	void Close();
	void Set_text(const std::string& t);
	const std::string& Get_text() const;

	Widget* Get_child() const;

	virtual void Organise();
	virtual void Handle_event(const ALLEGRO_EVENT& event);
	virtual void Handle_child_resize();
	virtual bool Add_child(Widget* c);
	virtual void Remove_child(Widget* c);
	virtual const std::string& Get_tooltip(float x, float y) const;

	enum Value
	{
		INDENT
	};
private:
	virtual void Resized();
	bool open;
	std::string text;
	Widget* child;
};

#endif