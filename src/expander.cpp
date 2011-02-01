#include "expander.h"
#include "widget_view.h"
#include "event_queue.h"
#include <sinxml/sinxml.h>

Expander::Expander()
:open(false)
,child(NULL)
{
}

Widget* Expander::Clone() const
{
	return new Expander(*this);
}

bool Expander::Is_open() const
{
	return open;
}

void Expander::Open()
{
	if(!open)
	{
		open = true;
		Child_resized();
	}
}

void Expander::Close()
{
	if(open)
	{
		open = false;
		Child_resized();
	}
}

void Expander::Set_text(const std::string& t)
{
	text = t;
}

const std::string& Expander::Get_text() const
{
	return text;
}

Widget* Expander::Get_child() const
{
	return child;
}

void Expander::Organise()
{
	float indent = Get_value(INDENT);
	Vector2 p = Get_position();
	p.y += Get_value(SELF_HEIGHT);
	Vector2 s = Get_size();
	s.y -= Get_value(SELF_HEIGHT);

	if(child)
	{
		child->Set_size(s);
		child->Set_position(p);
	}
}

void Expander::Handle_event(const ALLEGRO_EVENT& event)
{
	if(child && Is_open())
		child->Handle_event(event);

	if(ALLEGRO_EVENT_MOUSE_BUTTON_UP == event.type)
	{
		Vector2 p = Get_position();
		bool covers_point = Covers_point(event.mouse.x, event.mouse.y);

		if(covers_point && event.mouse.y<p.y+Get_value(SELF_HEIGHT))
		{
			if(event.mouse.x < p.x+Get_value(INDENT))
			{
				if(Is_open())
					Close();
				else
					Open();
				Child_resized();
			}
		}
	}
}

void Expander::Resized()
{
	Organise();
}

void Expander::Handle_child_resize()
{
	Resized();
	Child_resized();
}

bool Expander::Add_child(Widget* c)
{
	if(!child)
	{
		child = c;
		child->Set_parent(this);
		Organise();
		return true;
	}
	return false;
}

void Expander::Remove_child(Widget* c)
{
	child = NULL;
	Organise();
}

const std::string& Expander::Get_tooltip(float x, float y) const
{
	if(child && child->Covers_point(x, y))
		return child->Get_tooltip(x, y);
	return Widget::Get_tooltip(x, y);
}

using namespace sinxml;
sinxml::Element* Expander::To_xml() const
{
	Element* e_container = Container::To_xml();
	if(!e_container)
		return NULL;
	Element* e_self = new Element("expander");
	Element* e_base = new Element("base");
	e_self->Add_child(e_base);
	e_base->Add_child(e_container);

	Element* e_child_wrapper = new Element("child");
	e_self->Add_child(e_child_wrapper);
	if(child)
	{
		Element* e_child = child->To_xml();
		if(e_child)
		{
			e_child_wrapper->Add_child(e_child);
		}
	}
	Element* e_text = new Element("text", text);
	e_self->Add_child(e_text);
	return e_self;
}
