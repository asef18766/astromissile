#include "factory.h"
#include <iostream>
#define RADIUS 100
DEBUG(void factory::check_products(){
    DEBUG_OUTPUT(
      "factory::factory_count: "<< factory::factory_count<< '\n'<<
      "ball::ball_num: "<< ball::ball_num<< '\n'<<
      "factory::player_count: "<< factory::player_count<<'\n';)
})

int factory::factory_count=0;
int factory::ball_count = ball::ball_num;
int factory::player_count = player::player_num;

factory* factory::pFactory=nullptr;

factory::factory(){++factory_count;}
factory::~factory(){
    --factory_count;
    if(pFactory==this)
        pFactory=nullptr;
}
factory* factory::get_factory()
{
    if(pFactory==nullptr)
        pFactory=new factory();
    return pFactory;
}
ball* factory::get_ball()
{
    ball *b=new ball();
    return b;
}
player* factory::get_player()
{
    ball *player_ball=get_ball();
    player_ball->radius*=2;
    player *p=new player(player_ball);
    return p;
}
object::object():
  center (new pos),c_v (new vec),type (),kid (nullptr),kid_num (),_i (factory::get_factory()->get_ball()),routine (),dR ()
{}

//For copying.
object::object(const object &val):
  center (new pos),c_v (new vec),type (val.type),kid (nullptr),kid_num (val.kid_num),_i (factory::get_factory()->get_ball()),routine (val.routine),radius (val.radius),dR (val.dR)
{
  *center=*val.center;
  *c_v=*val.c_v;

  if (kid_num>0){
      kid = new object[kid_num];
      for (int k=0; k<kid_num; ++k)
      {
          kid[k]=val.kid[k];
      }
  }
  *_i=*val._i;
}

//For assignment.
object &object::operator=(const object &val)
{
  if (this == &val){return *this;}

  if (center==nullptr){
      center=new pos;
  }
  *center=*val.center;

  if (c_v==nullptr){
      c_v=new vec;
  }
  *c_v=*val.c_v;

  type=val.type;

  kid_num=val.kid_num;
  if (kid_num>0){
      kid = new object[kid_num];
      for (int k=0; k<kid_num; ++k)
      {
          kid[k]=val.kid[k];
      }
  }
  else
  {
      kid=nullptr;
  }

  *_i=*val._i;
  routine=val.routine;
  radius=val.radius;
  dR=val.dR;
  return *this;
}

//for the initial missile
object::object(pos _center,vec _c_v,char *type_str):
  center (new pos),c_v (new vec),type (type_str[0]),kid (nullptr),kid_num (),_i (factory::get_factory()->get_ball()),routine ()
{
    DEBUG(std::cout<<"[object::object(pos _center,vec _c_v,char *type_str)]\n";)
    (*center)=_center;
    c_v->x=_c_v.x/_c_v.length();c_v->y=_c_v.y/_c_v.length();

    radius=RADIUS;
    dR=1;

    int cur_pos=1;
    if(type=='n')
    {
        DEBUG(std::cout<<"type:n\n";)
        kid_num=0;
        kid=nullptr;
    }
    else if(type=='m')
    {
        DEBUG(std::cout<<"type:m\n";)
        type='m';
        routine=0;
        kid_num=2;
        kid=new object[2];
        vec a,b;
        a.x=acos(radianToDegree(90.0)),a.y=asin(radianToDegree(90.0));
        b.x=acos(radianToDegree(-90.0)),b.y=asin(radianToDegree(-90.0));
        kid[0].c_v=new vec;
        kid[1].c_v=new vec;

        *kid[0].c_v=rotation(*this->c_v,a);
        *kid[1].c_v=rotation(*this->c_v,b);

        kid[0].center=new pos;
        kid[1].center=new pos;

        *kid[0].center=*this->center;
        *kid[1].center=*this->center;

        for(int q=0;q!=kid_num;++q)
        {
            kid[q].dR=dR*0.5;
            kid[q].radius=radius/1.5;
        }

        kid[0].object_initialize(type_str,cur_pos);
        kid[1].object_initialize(type_str,cur_pos);
    }
    else if(type=='s')
    {
        dR*=10;
        routine=0;
        kid_num=0;

        for(;type_str[cur_pos]>='0'&&type_str[cur_pos]<='9';++cur_pos)
        {
            kid_num=(kid_num*10+type_str[cur_pos]-'0');
            DEBUG(std::cout<<"kid_num:"<<kid_num<<std::endl;)
            DEBUG(std::cout<<"cur_c:"<<type_str[cur_pos]<<std::endl;)
        }
        kid=new object[kid_num];

        for(int u=0;u!=kid_num;++u)
        {
            kid[u].dR=dR;
            kid[u].radius=radius/1.5;

            kid[u].c_v=new vec;
            kid[u].center=new pos;

            kid[u].center->x=center->x+radius*cos(radianToDegree(360.0/kid_num*u));
            kid[u].center->y=center->y+radius*sin(radianToDegree(360.0/kid_num*u));

            kid[u].c_v->x=cos(radianToDegree(360.0/kid_num*u+90.0));
            kid[u].c_v->y=sin(radianToDegree(360.0/kid_num*u+90.0));

            kid[u].object_initialize(type_str,cur_pos);
        }
    }
    else
        while(1){std::cout<<"object::object(pos _center,vec _c_v,char *type_str) Error!\r";}
}
void object::object_initialize(char *type_str,int &cur_pos)
{
    DEBUG(std::cout<<"[void object::object_initialize]\n";)
    cur_pos++;
    if(type_str[cur_pos-1]=='n')
    {
        DEBUG(std::cout<<"type:n\n";)
        type='n';
        kid_num=0;
        kid=nullptr;
    }
    else if(type_str[cur_pos-1]=='m')
    {
        DEBUG(std::cout<<"type:m\n";)
        type='m';
        routine=0;
        kid_num=2;
        kid=new object[2];

        kid[0].c_v=new vec;
        kid[1].c_v=new vec;

        *kid[0].c_v=*this->c_v;
        *kid[1].c_v=*this->c_v;

        kid[0].center=new pos;
        kid[1].center=new pos;

        *kid[0].center=*this->center;
        *kid[1].center=*this->center;

        for(int q=0;q!=kid_num;++q)
        {
            kid[q].dR=dR*0.5;
            kid[q].radius=radius/1.5;
        }

        kid[0].object_initialize(type_str,cur_pos);
        kid[1].object_initialize(type_str,cur_pos);
    }
    else if(type_str[cur_pos-1]=='s')
    {
        DEBUG(std::cout<<"type:s\n";)
        type='s';
        routine=0;
        kid_num=0;

        for(;type_str[cur_pos]>='0'&&type_str[cur_pos]<='9';++cur_pos)
        {
            kid_num=(kid_num*10+type_str[cur_pos]-'0');
            DEBUG(std::cout<<"kid_num:"<<kid_num<<std::endl;)
            DEBUG(std::cout<<"cur_c:"<<type_str[cur_pos]<<std::endl;)
        }
        kid=new object[kid_num];

        for(int u=0;u!=kid_num;++u)
        {
            kid[u].dR=dR/2;
            kid[u].radius=radius/1.5;

            kid[u].c_v=new vec;
            kid[u].center=new pos;

            kid[u].center->x=center->x+radius*cos(radianToDegree(360.0/kid_num*u));
            kid[u].center->y=center->y+radius*sin(radianToDegree(360.0/kid_num*u));

            kid[u].c_v->x=cos(radianToDegree(360.0/kid_num*u+90.0));
            kid[u].c_v->y=sin(radianToDegree(360.0/kid_num*u+90.0));

            kid[u].object_initialize(type_str,cur_pos);
        }
    }
    else
    {
        std::cout<<"init_Error!\r";
        while(1){}
    }
}
void object::update(double V)
{
    DEBUG(std::cout<<"[void object::update]\n";)
    center->change_pos_by_V(c_v->x*V,c_v->y*V);
    if(type=='n');
    else if(type=='m')
    {
        routine+=(dR/7);

        vec dv,a,b;
        dv=(*c_v)*sin(routine)*0.5;

        a.x=-dv.y;
        a.y=dv.x;

        b.x=dv.y;
        b.y=-dv.x;

        *kid[0].c_v=a+*c_v;
        *kid[1].c_v=b+*c_v;
        pos p1=*center,p2=p1;
        p1.change_pos_by_V(a.x*radius,a.y*radius);
        p2.change_pos_by_V(b.x*radius,b.y*radius);
        *kid[0].center=p1;
        *kid[1].center=p2;

        kid[0].update(V);
        kid[1].update(V);
    }
    else if(type=='s')
    {
        routine+=dR;
        for(int u=0;u!=kid_num;++u)
        {
            kid[u].center->x=center->x+radius*cos(radianToDegree(360.0/kid_num*u+routine));
            kid[u].center->y=center->y+radius*sin(radianToDegree(360.0/kid_num*u+routine));

            kid[u].c_v->x=cos(radianToDegree(360.0/kid_num*u+90.0+routine));
            kid[u].c_v->y=sin(radianToDegree(360.0/kid_num*u+90.0+routine));

            kid[u].update(V);
        }
    }
    else
        while(1){std::cout<<"update_Error!\r";}
}
void object::draw(SDL_Renderer* pRenderer)
{
    DEBUG(std::cout<<"[void object::draw]\n";)
    DEBUG(std::cout<<"draw_type:"<<type<<'\n';)

    _i->change_pos(center->x,center->y);
    if(type=='n')
        _i->draw_ball(pRenderer);
    else if(type=='m')
    {
        ball::rgba b1,b2,b3;
        b1.a=b2.a=b3.a=255;
        b1.r=b1.g=b1.b=0;
        b2.r=b2.g=b2.b=255;
        b3.r=b3.g=b3.b=128;

        kid[0]._i->color=b1;
        kid[1]._i->color=b2;

        kid[0].draw(pRenderer);
        kid[1].draw(pRenderer);

        _i->color=b3;
        _i->change_pos(center->x,center->y);
        _i->draw_ball(pRenderer);
    }
    else if(type=='s')
    {
        ball::rgba b1,b2,b3;
        b1.a=b2.a=b3.a=255;
        b1.r=b1.g=b1.b=0;
        b2.r=b2.g=b2.b=255;
        b3.r=b3.g=b3.b=128;

        for(int u=0;u!=kid_num;++u)
            kid[u]._i->color=b1;
        for(int u=0;u!=kid_num;++u)
            kid[u].draw(pRenderer);

        _i->color=b3;
        _i->change_pos(center->x,center->y);
        _i->draw_ball(pRenderer);
    }
    else
    {
        std::cout<<"draw_Error!\r";
        while(1){}
    }
}
bool object::is_out_of_boarder(int _x,int _y)
{
    DEBUG(std::cout<<"[bool object::is_out_of_boarder]\n";)
    return center->x < -200 || center->y < -200 ||
             center->x > _x+200 || center->y > _y+200;
}
object::~object()
{
    if(_i!=nullptr)
        delete _i;
    if(kid!=nullptr)
        delete [] kid;
    if(center!=nullptr)
        delete center;
    if(c_v!=nullptr)
        delete c_v;
}
