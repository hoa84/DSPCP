/*
**  Data Scalable Approach for Parallel Coordinates
**  Copyright (C) 2016 - Hoa Nguyen, Paul Rosen
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <SCI/Graphics/RenderElements.h>

#include <DarkView/SmallMultiples.h>

#include <GL/oglCommon.h>

SmallMultiples::SmallMultiples(oglWidgets::oglFont &_font ) {
    font = &_font;
    mouse_x = mouse_y = FLT_MAX;
    selX = selY = 0;
    mouse_selX = mouse_selY = -1;
}

void SmallMultiples::SetMouse( float mx, float my ){
    mouse_x = mx;
    mouse_y = my;
}

void SmallMultiples::Select( ){
    if( mouse_selX != -1 && mouse_selY != -1 ){
        selX = mouse_selX;
        selY = mouse_selY;
    }
}

std::pair<int,int> SmallMultiples::GetSelected( ){
    if( mouse_selX != -1 && mouse_selY != -1 ){
        return std::make_pair(mouse_selX,mouse_selY);
    }
    return std::make_pair(selX,selY);
}

void SmallMultiples::Reset( ){
    for(int i = 0; i < (int)sp.size(); i++){
        sp[i].Reset();
    }
}

void SmallMultiples::ProgressiveReset(){
    for(int i = 0; i < sp.size(); i++){
        sp[i].ProgressiveReset();
    }
}

bool SmallMultiples::ProgressiveDraw( DataIndirector & data ){

    float startX = -0.95f;
    float endX   =  0.80f;
    float startY = -0.95f;
    float endY   =  0.85f;

    float scaleX = (endX-startX)/(float)(data.GetDim()-1);
    float scaleY = (endY-startY)/(float)(data.GetDim()-0);

    mouse_selX = mouse_selY = -1;

    bool draw_anything = false;

    for(int i = 0, k = 0; i < data.GetDim(); i++){
        for(int j = 0; j < i; j++, k++){
            if(k >= (int)sp.size()) sp.push_back( ScatterPlot(*font) );
            sp[k].SetCenter(startX+((float)i-1.0f+0.5f)*scaleX,startY+((float)j+0.5f)*scaleY);
            sp[k].SetSize(0.9f*scaleX,0.9f*scaleY);
            sp[k].SetBorderWidth(1.0f);
            sp[k].SetBorderColor(0.0f,0.0f,0.0f);
            if( sp[k].DistanceToObject( mouse_x, mouse_y ) < 0.00001f ){
                mouse_selX = i;
                mouse_selY = j;
                sp[k].SetBorderWidth(1.0f);
                sp[k].SetBorderColor(1.0f,0.0f,0.0f);
            }
            if( selX == i && selY == j ){
                sp[k].SetBorderWidth(2.0f);
                sp[k].SetBorderColor(1.0f,0.0f,0.0f);
            }
            sp[k].Set( data, i, j, false );
            //sp[k].SetAspect( aspect );
            draw_anything = sp[k].ProgressiveDraw() || draw_anything;

        }
    }

    return draw_anything;

}

void SmallMultiples::ProgressiveBorder( DataIndirector & data ){

    float startX = -0.95f;
    float endX   =  0.80f;
    float startY = -0.95f;
    float endY   =  0.85f;

    float scaleX = (endX-startX)/(float)(data.GetDim()-1);
    float scaleY = (endY-startY)/(float)(data.GetDim()-0);

    mouse_selX = mouse_selY = -1;

    for(int i = 0, k = 0; i < data.GetDim(); i++){
        for(int j = 0; j < i; j++, k++){
            if(k >= (int)sp.size()) sp.push_back( ScatterPlot(*font) );
            sp[k].SetCenter(startX+((float)i-1.0f+0.5f)*scaleX,startY+((float)j+0.5f)*scaleY);
            sp[k].SetSize(0.9f*scaleX,0.9f*scaleY);
            sp[k].SetBorderWidth(1.0f);
            sp[k].SetBorderColor(0.0f,0.0f,0.0f);
            if( sp[k].DistanceToObject( mouse_x, mouse_y ) < 0.00001f ){
                mouse_selX = i;
                mouse_selY = j;
                sp[k].SetBorderWidth(1.0f);
                sp[k].SetBorderColor(1.0f,0.0f,0.0f);
            }
            if( selX == i && selY == j ){
                sp[k].SetBorderWidth(3.0f);
                sp[k].SetBorderColor(1.0f,0.0f,0.0f);
            }
            sp[k].Set( data, i, j, false );
            //sp[k].SetAspect( aspect );
            sp[k].ProgressiveBorder();

        }
    }
}



void SmallMultiples::ProgressiveLabels(float aspect, DataIndirector & data ){

    float startX = -0.95f;
    float endX   =  0.80f;
    float startY = -0.95f;
    float endY   =  0.85f;

    float scaleX = (endX-startX)/(float)(data.GetDim()-1);
    float scaleY = (endY-startY)/(float)(data.GetDim()-0);

    SCI::Graphics::TextElement text( font );

    text.SetColor(0,0,0);
    text.SetSize( 0.05f );
    text.SetAspect( aspect );


    glColor3f(0,0,0);
    glPushMatrix();
        glTranslatef( startX+scaleX/2.0f, startY+scaleY*1.1f + 0.005f, 0.0f );
        for(int i = 1; i < data.GetDim(); i++){
            glPushMatrix();
                glScalef(1,1,1);
                text.SetOrientation( 0.0f);
                text.SetCenter( 0.005f );
                text.SetTop(   0 );
                text.SetText( data.GetLabelParsed(i).c_str() );
                text.Draw();
            glPopMatrix();

            glTranslatef( scaleX, scaleY, 0.0f );
        }
    glPopMatrix();

    glPushMatrix();
        glTranslatef( endX, startY+scaleY/2.0f, 0.0f );
        for(int i = 0; i < data.GetDim()-1; i++){
            glPushMatrix();
                glScalef(1,1,1);
                text.SetOrientation( 0.0f);
                text.SetLeft( 0 );
                text.SetMiddle(  0.01f );
                text.SetText( data.GetLabelParsed(i).c_str() );
                text.Draw();
            glPopMatrix();

            glTranslatef( 0.0f, scaleY, 0.0f );
        }
    glPopMatrix();

}
