#include "tree.h"
#include "widget_view.h"
#include "event_queue.h"
#include <algorithm>

Tree::Tree()
:open(false)
,selected(false)
{
}

Widget* Tree::Clone() const
{
	return new Tree(*this);
}

bool Tree::Is_open() const
{
	return open;
}

void Tree::Open()
{
	open = true;
}

void Tree::Close()
{
	open = false;
}

void Tree::Set_text(const std::string& t)
{
	text = t;
}

const std::string& Tree::Get_text() const
{
	return text;
}

void Tree::Add_child(Tree* c)
{
	if(!c)
		return;
	children.push_back(c);
	c->Set_parent(this);
	Organise();
	Child_resized();
}

void Tree::Remove_child(Tree* c)
{
	Trees::iterator i = std::find(children.begin(), children.end(), c);
	if(i!=children.end())
		children.erase(i);
	Organise();
	Child_resized();
}

const Trees& Tree::Get_children() const
{
	return children;
}

Trees& Tree::Get_children()
{
	return children;
}

bool Tree::Is_selected() const
{
	return selected;
}

void Tree::Child_selected()
{
	Tree* parent = dynamic_cast<Tree*>(Get_parent());
	if(parent)
		parent->Child_selected();
	Deselect();
}

void Tree::Deselect()
{
	bool event_deselected = selected;
	selected = false;
	for(Trees::iterator i = children.begin(); i != children.end(); ++i)
	{
		(*i)->Deselect();
	}
	if(event_deselected)
		Push_event(Event(this, "deselected"));
}

void Tree::Organise()
{
	bool is_open = Is_open();
	Close();
	Vector2 selfsize = Request_size();
	if(is_open)
		Open();

	float indent = Get_value(INDENT);
	float y = Get_position().y;
	float x = Get_position().x + indent;
	y += selfsize.y;

	for(Trees::iterator i = children.begin(); i != children.end(); ++i)
	{
		Vector2 size = (*i)->Request_size();
		(*i)->Set_size(size);
		(*i)->Set_position(Vector2(x, y));
		y += size.y;
	}
}

void Tree::Handle_event(const ALLEGRO_EVENT& event)
{
	if(Is_open())
	{
		for(Trees::iterator i = children.begin(); i != children.end(); ++i)
		{
			(*i)->Handle_event(event);
		}
	}

	if(ALLEGRO_EVENT_MOUSE_BUTTON_UP == event.type)
	{
		bool is_open = Is_open();
		Vector2 ps = Get_size();
		if(is_open)
		{
			Close();
			Set_size(Request_size());
		}
		Vector2 p = Get_position();
		Vector2 s = Get_size();
		bool covers_point = Covers_point(event.mouse.x, event.mouse.y);
		if(is_open)
		{
			Open();
			Set_size(ps);
		}
		
		if(covers_point)
		{
			if(event.mouse.x < p.x+Get_value(INDENT))
			{
				if(is_open)
					Close();
				else
					Open();
				Child_resized();
			}
			else
			{
				Child_selected();
				selected = true;
				Push_event(Event(this, "selected"));
			}
		}
	}
}

void Tree::Resized()
{
	Organise();
}

void Tree::Handle_child_resize()
{
	Resized();
	Child_resized();
}

bool Tree::Add_child(Widget* c)
{
	Tree* e = dynamic_cast<Tree*>(c);
	if(e)
	{
		Add_child(e);
		return true;
	}
	return false;
}

void Tree::Remove_child(Widget* c)
{
	Trees::iterator i = std::find(children.begin(), children.end(), c);
	if(i != children.end())
	{
		children.erase(i);
	}
	Organise();
}

const std::string& Tree::Get_tooltip(float x, float y) const
{
	for(Trees::const_iterator i = children.begin(); i != children.end(); ++i)
	{
		if((*i)->Covers_point(x, y))
			return (*i)->Get_tooltip(x, y);
	}
	return Widget::Get_tooltip(x, y);
}