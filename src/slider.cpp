#include "slider.h"
#include "event_queue.h"
#include <iostream>

Slider::Slider()
:pane_position(0)
,pane_reference(0.0)
,pane_size(30)
,holding_pane(-1)
,scroll_units(10)
{
}

float Slider::Get_pane_position() const
{
	return pane_position;
}

float Slider::Get_pane_fraction() const
{
	return pane_reference;
}

void Slider::Set_pane_size(int s)
{
	pane_size = s;
}

int Slider::Get_pane_size() const
{
	return pane_size;
}

void Slider::Set_scroll_units(float su)
{
	scroll_units = su;
}

float Slider::Get_scroll_units() const
{
	return scroll_units;
}

void Slider::Resized()
{
	Set_pane_fraction(pane_reference);
}
