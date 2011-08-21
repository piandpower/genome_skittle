#include "Color.h"

bool Color::operator == (color other)
{
    return ( (r == other.r) && (g == other.g) && (b == other.b) );
}
color Color::operator + (color other)
{
    return color( min(255, r + other.r), min(255, g + other.g), min(255, b + other.b) );
}
color Color::operator - (color other)
{
    return color( max(0,(r - other.r)), max(0,(g - other.g)), max(0,(b - other.b)) );
}
color Color::operator / (int div)
{
    return color( (int)(float(r)/div+.5), (int)(float(g)/div+.5), (int)(float(b)/div+.5) );
}
color Color::operator * (float mul)
{
    return color( min(255, (int)(float(r)*mul+.5)), min(255, (int)(float(g)*mul+.5)), min(255, (int)(float(b)*mul+.5)) );
}
int Color::lightness()
{
    int a = max(r, g);
    return max( a, b);
}

void Color::setRed(int red){
    r = min(255,red);
}
void Color::setGreen(int green){
    g = min(255,green);
}
void Color::setBlue(int blue){
    b = min(255,blue);
}

int Color::getRed(){
    return r;
}
int Color::getGreen(){
    return g;
}
int Color::getBlue(){
    return b;
}