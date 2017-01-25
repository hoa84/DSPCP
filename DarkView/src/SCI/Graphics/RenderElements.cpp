/*
**  Common Library
**  Copyright (C) 2013  Paul Rosen
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
#include <SCI/BoundingBox.h>
#include <SCI/Mat4.h>

#include <GL/oglCommon.h>

#include <iostream>

using namespace SCI::Graphics;

RenderElement::RenderElement( ) : user_data(0), depth(0) { }
RenderElement::~RenderElement(){ }
void RenderElement::SetTooltip( const char * str ){ tooltip = str; }
void RenderElement::SetTooltip( std::string  str ){ tooltip = str; }
std::string RenderElement::GetTooltip(){ return tooltip; }
void RenderElement::SetDepth( float d ){ depth = d; }
void   RenderElement::SetUserData( void * v ){ user_data = v; }
void * RenderElement::GetUserData( ){ return user_data; }


RectangularRenderElement::RectangularRenderElement( ) : type(0), line(2), angle(0) { }
void RectangularRenderElement::SetLineWidth(float val){ line = val; }
void RectangularRenderElement::SetSize( float sx, float sy ){ size.Set( sx, sy ); }
void RectangularRenderElement::SetSize( float s ){ size.Set( s, s ); }
void RectangularRenderElement::SetCenter( float cx, float cy ){ center.Set( cx, cy ); }
void RectangularRenderElement::SetOrientation( float _angle ){ angle = _angle; }
void RectangularRenderElement::SetSolid( bool v, float r, float g, float b ){ (v)?(type|=0x01):(type&=~0x01); s_color.Set(r,g,b); }
void RectangularRenderElement::SetSolid( bool v, SCI::Vex3 col ){ (v)?(type|=0x01):(type&=~0x01); s_color=col; }
void RectangularRenderElement::SetEmpty( bool v, float r, float g, float b ){ (v)?(type|=0x02):(type&=~0x02); e_color.Set(r,g,b); }
void RectangularRenderElement::SetEmpty( bool v, SCI::Vex3 col ){ (v)?(type|=0x02):(type&=~0x02); e_color=col; }
void RectangularRenderElement::SetDropShadow( bool v, float offset, float depth ){ (v)?(type|=0x04):(type&=~0x04); s_offset = offset; s_depth = depth; }

float RectangularRenderElement::DistanceToObject( float x, float y ){
    float dx = ::fabsf( x-center.x ) - size.x/2.0f;
    float dy = ::fabsf( y-center.y ) - size.y/2.0f;
    return sqrtf( ( (dx<=0)?(0):(dx*dx) ) + ( (dy<=0)?(0):(dy*dy) ) );
}



TextElement::TextElement( oglWidgets::oglFont * _font ) : font( _font ), d_shadow(false), s_offset(0), s_depth(0), j_type(0), angle(0), aspect(1) { }
void TextElement::SetText( const char * _txt ){ txt = _txt; }
void TextElement::SetTextf( const char *fmt, ... ){
    char        text[1024] = "";        // Holds Our String
    va_list        ap;                // Pointer To List Of Arguments

    txt = "";

    if (fmt == NULL) return;                // If There's No Text

    va_start(ap, fmt);                    // Parses The String For Variables
        vsprintf(text, fmt, ap);        // And Converts Symbols To Actual Numbers
        txt += text;
    va_end(ap);                        // Results Are Stored In Text
}

void TextElement::SetSize( float s ){ size.Set( s, s ); }

void TextElement::SetCenter( float cx ){ j_type = (j_type&0xf0) + 0x00; center.x = cx; }
void TextElement::SetLeft( float lx ){   j_type = (j_type&0xf0) + 0x01; center.x = lx; }
void TextElement::SetRight( float rx ){  j_type = (j_type&0xf0) + 0x02; center.x = rx; }
void TextElement::SetTop( float ty ){    j_type = (j_type&0x0f) + 0x10; center.y = ty; }
void TextElement::SetMiddle( float my ){ j_type = (j_type&0x0f) + 0x00; center.y = my; }
void TextElement::SetBottom( float by ){ j_type = (j_type&0x0f) + 0x20; center.y = by; }

void TextElement::SetAspect( float w_div_h ){ aspect = w_div_h; }
void TextElement::SetOrientation( float _angle ){ angle = _angle; }
void TextElement::SetColor( float r, float g, float b ){ color.Set(r,g,b); }
float TextElement::DistanceToObject( float, float ){ return FLT_MAX; }
void TextElement::SetDropShadow( bool v, float offset, float depth ){ d_shadow = v; s_offset = offset; s_depth = depth; }

void TextElement::Draw( ){
    float width = font->GetWidthf( txt.c_str() ) * size.x;
    float height = size.y;

    SCI::Mat4 rot( SCI::Mat4::ROTATION, angle, 0, 0, 1 );

    SCI::BoundingBox bb;
    bb.Expand( rot*SCI::Vex3(  width/2.0f,  height/2.0f, 0 ) );
    bb.Expand( rot*SCI::Vex3(  width/2.0f, -height/2.0f, 0 ) );
    bb.Expand( rot*SCI::Vex3( -width/2.0f,  height/2.0f, 0 ) );
    bb.Expand( rot*SCI::Vex3( -width/2.0f, -height/2.0f, 0 ) );

    glPushMatrix();
        glColor3f( color.r, color.g, color.b );

        // Move to location
        glTranslatef( center.x, center.y, 0 );

        // Correct for aspect ratio
        glScalef( 1.0f/aspect, 1.0f, 1.0f );

        // Justify text
        if( (j_type&0x0f) == 0x01 ){ glTranslatef( bb.bbmax.x, 0,0 ); } // left justify
        if( (j_type&0x0f) == 0x02 ){ glTranslatef( bb.bbmin.x, 0,0 ); } // right justify
        if( (j_type&0xf0) == 0x10 ){ glTranslatef( 0, bb.bbmin.y, 0 ); } // top justify
        if( (j_type&0xf0) == 0x20 ){ glTranslatef( 0, bb.bbmax.y, 0 ); } // bottom justify

        // Apply rotation
        glRotatef( angle, 0,0,1 );

        // Translate to center
        glTranslatef( -width/2.0f, -height/2.0f, depth );

        // Scale to size
        glScalef( size.x, size.y, 0.001f );

        // Draw Text
        font->Print( txt.c_str() );
    glPopMatrix();

    // Drop Shadow
    if( d_shadow ){
        glPushMatrix();
            glTranslatef( s_offset, -s_offset, s_depth );
            glColor3f( 0.65f, 0.65f, 0.65f );
            glTranslatef( center.x, center.y, 0 );
            if( (j_type&0x0f) == 0x01 ){ glTranslatef( bb.bbmax.x, 0,0 ); } // left justify
            if( (j_type&0x0f) == 0x02 ){ glTranslatef( bb.bbmin.x, 0,0 ); } // right justify
            if( (j_type&0xf0) == 0x10 ){ glTranslatef( 0, bb.bbmin.y, 0 ); } // top justify
            if( (j_type&0xf0) == 0x20 ){ glTranslatef( 0, bb.bbmax.y, 0 ); } // bottom justify
            glScalef( 1.0f/aspect, 1.0f, 1.0f );
            glRotatef( angle, 0,0,1 );
            glTranslatef( -width/2.0f, -height/2.0f, 0 );
            glScalef( size.x / aspect, size.y, 0.001f );
            font->Printf( txt.c_str() );
        glPopMatrix();
    }

}

/*
BinnedRectangularElement::BinnedRectangularElement( RenderBuffer * rb ) : RenderableRectangularElement( rb ){ }

void BinnedRectangularElement::SetBinCount( int cnt ){ bcols.resize( cnt, s_color ); }
void BinnedRectangularElement::SetBinColor( int bin, float r, float g, float b ){ bcols[bin].Set(r,g,b); }
void BinnedRectangularElement::SetBinColor( int bin, SCI::Vex3 col ){ bcols[bin] = col; }

void BinnedRectangularElement::Draw( ){
    render_buffer->LineWidth( line );
    render_buffer->PushMatrix();
        render_buffer->Color( s_color.r, s_color.g, s_color.b );
        render_buffer->Translate( center.x, center.y, depth );
        render_buffer->Rotate( angle, 0,0,1 );
        for(int i = 0; i < bcols.size(); i++){
            float x0 = lerp( -size.x/2.0f, size.x/2.0f, (float)(i+0)/(float)bcols.size() );
            float x1 = lerp( -size.x/2.0f, size.x/2.0f, (float)(i+1)/(float)bcols.size() );
            float y0 = -size.y/2.0f;
            float y1 =  size.y/2.0f;
            if( (type&0x01)!=0 ){ Renderable::SetColor(bcols[i].r,bcols[i].g,bcols[i].b); Renderable::DrawSolidRect( SCI::Vex2( x0, y0 ), SCI::Vex2( x1, y1 ) ); }
            if( (type&0x02)!=0 ){ Renderable::SetColor( e_color.r, e_color.g, e_color.b); Renderable::DrawEmptyRect( SCI::Vex2( x0, y0 ), SCI::Vex2( x1, y1 ) ); }
        }
    render_buffer->PopMatrix();

    // Drop Shadow
    if( (type&0x04)!=0 ){
        render_buffer->PushMatrix();
            render_buffer->Translate( s_offset, -s_offset, s_depth );
            render_buffer->Color( s_color.r, s_color.g, s_color.b );
            render_buffer->Translate( center.x, center.y, 0.0f );
            render_buffer->Rotate( angle, 0,0,1 );
            Renderable::SetColor(0.65f,0.65f,0.65f);
            if( (type&0x01)!=0 ){ DrawSolidRect( -size/2.0f, size/2.0f ); }
            if( (type&0x02)!=0 ){ DrawEmptyRect( -size/2.0f, size/2.0f ); }
        render_buffer->PopMatrix();
    }
}






RectElement::RectElement( MyLib::RenderBuffer * rb ) : RenderableRectangularElement( rb ) { }
void RectElement::Draw( ){
    render_buffer->LineWidth( line );
    render_buffer->PushMatrix();
        render_buffer->Color( s_color.r, s_color.g, s_color.b );
        render_buffer->Translate( center.x, center.y, depth );
        render_buffer->Rotate( angle, 0,0,1 );
        if( (type&0x01)!=0 ){ Renderable::SetColor(s_color.r,s_color.g,s_color.b); DrawSolidRect( -size/2.0f, size/2.0f ); }
        if( (type&0x02)!=0 ){ Renderable::SetColor(e_color.r,e_color.g,e_color.b); DrawEmptyRect( -size/2.0f, size/2.0f ); }
    render_buffer->PopMatrix();

    // Drop Shadow
    if( (type&0x04)!=0 ){
        render_buffer->PushMatrix();
            render_buffer->Translate( s_offset, -s_offset, s_depth );
            render_buffer->Color( s_color.r, s_color.g, s_color.b );
            render_buffer->Translate( center.x, center.y, 0.0f );
            render_buffer->Rotate( angle, 0,0,1 );
            Renderable::SetColor(0.65f,0.65f,0.65f);
            if( (type&0x01)!=0 ){ DrawSolidRect( -size/2.0f, size/2.0f ); }
            if( (type&0x02)!=0 ){ DrawEmptyRect( -size/2.0f, size/2.0f ); }
        render_buffer->PopMatrix();
    }
}

PlusElement::PlusElement( MyLib::RenderBuffer * rb ) : RenderableRectangularElement( rb ){ }

void PlusElement::Draw( ){
    render_buffer->LineWidth( line );
    render_buffer->PushMatrix();
        render_buffer->Color( s_color.r, s_color.g, s_color.b );
        render_buffer->Translate( center.x, center.y, depth );
        render_buffer->Rotate( angle, 0,0,1 );
        if( (type&0x01)!=0 ){
            Renderable::SetColor(s_color.r,s_color.g,s_color.b);
            DrawSolidRect( -SCI::Vex2(size.x/10.0f,size.y/ 2.0f), SCI::Vex2(size.x/10.0f,size.y/ 2.0f) );
            DrawSolidRect( -SCI::Vex2(size.x/ 2.0f,size.y/10.0f), SCI::Vex2(size.x/ 2.0f,size.y/10.0f) );
        }
        if( (type&0x02)!=0 ){
            Renderable::SetColor(e_color.r,e_color.g,e_color.b);
            DrawEmptyRect( -SCI::Vex2(size.x/10.0f,size.y/ 2.0f), SCI::Vex2(size.x/10.0f,size.y/ 2.0f) );
            DrawEmptyRect( -SCI::Vex2(size.x/ 2.0f,size.y/10.0f), SCI::Vex2(size.x/ 2.0f,size.y/10.0f) );
        }
    render_buffer->PopMatrix();

    // Drop Shadow
    if( (type&0x04)!=0 ){
        render_buffer->PushMatrix();
            render_buffer->Translate( s_offset, -s_offset, s_depth );
            render_buffer->Color( 0.65f, 0.65f, 0.65f );
            render_buffer->Translate( center.x, center.y, depth );
            render_buffer->Rotate( angle, 0,0,1 );
            if( (type&0x01)!=0 ){
                DrawSolidRect( -SCI::Vex2(size.x/10.0f,size.y/ 2.0f), SCI::Vex2(size.x/10.0f,size.y/ 2.0f) );
                DrawSolidRect( -SCI::Vex2(size.x/ 2.0f,size.y/10.0f), SCI::Vex2(size.x/ 2.0f,size.y/10.0f) );
            }
            if( (type&0x02)!=0 ){
                DrawEmptyRect( -SCI::Vex2(size.x/10.0f,size.y/ 2.0f), SCI::Vex2(size.x/10.0f,size.y/ 2.0f) );
                DrawEmptyRect( -SCI::Vex2(size.x/ 2.0f,size.y/10.0f), SCI::Vex2(size.x/ 2.0f,size.y/10.0f) );
            }
        render_buffer->PopMatrix();
    }

}

StarElement::StarElement( MyLib::RenderBuffer * rb ) : RenderableRectangularElement( rb ) { }

void StarElement::Draw( ){
    render_buffer->LineWidth( line );
    render_buffer->PushMatrix();
        render_buffer->Translate( center.x, center.y, depth );
        render_buffer->Rotate( angle, 0,0,1 );
        if( (type&0x01)!=0 ){
            Renderable::SetColor(s_color.r,s_color.g,s_color.b);
            render_buffer->PushMatrix();
                for(int i = 0; i < 4; i++){
                    render_buffer->Rotate(45.0f,0,0,1);
                    DrawSolidRect( SCI::Vex2(size.x/10.0f,size.y/ 2.0f), SCI::Vex2(-size.x/10.0f,-size.y/ 2.0f) );
                }
            render_buffer->PopMatrix();
        }
        if( (type&0x02)!=0 ){
            Renderable::SetColor(e_color.r,e_color.g,e_color.b);
            render_buffer->PushMatrix();
                for(int i = 0; i < 4; i++){
                    render_buffer->Rotate(45.0f,0,0,1);
                    DrawEmptyRect( SCI::Vex2(size.x/10.0f,size.y/ 2.0f), SCI::Vex2(size.x/10.0f,size.y/ 2.0f) );
                }
            render_buffer->PopMatrix();
        }
    render_buffer->PopMatrix();

    // Drop Shadow
    if( (type&0x04)!=0 ){
        render_buffer->PushMatrix();
            render_buffer->Translate( s_offset, -s_offset, s_depth );
            render_buffer->Color( 0.65f,0.65f,0.65f );
            render_buffer->Translate( center.x, center.y, 0.0f );
            render_buffer->Rotate( angle, 0,0,1 );
            if( (type&0x01)!=0 ){
                render_buffer->PushMatrix();
                    for(int i = 0; i < 4; i++){
                        render_buffer->Rotate(45.0f,0,0,1);
                        DrawSolidRect( SCI::Vex2(size.x/10.0f,size.y/ 2.0f), SCI::Vex2(-size.x/10.0f,-size.y/ 2.0f) );
                    }
                render_buffer->PopMatrix();
            }
            if( (type&0x02)!=0 ){
                render_buffer->PushMatrix();
                    for(int i = 0; i < 4; i++){
                        render_buffer->Rotate(45.0f,0,0,1);
                        DrawEmptyRect( SCI::Vex2(size.x/10.0f,size.y/ 2.0f), SCI::Vex2(size.x/10.0f,size.y/ 2.0f) );
                    }
                render_buffer->PopMatrix();
            }
        render_buffer->PopMatrix();
    }

}


ExpandingLineElement::ExpandingLineElement( RenderBuffer * rb ) : RenderableElement( rb ), scale(1), approach(0.05f), d_shadow(false)  { }

void ExpandingLineElement::SetApproach( float a ){ approach = a; }
void ExpandingLineElement::SetStart( float sx0, float sy0, float w ){ pos0.Set( sx0, sy0 ); w0 = w; }
void ExpandingLineElement::SetEnd( float sx1, float sy1, float w ){ pos1.Set( sx1, sy1 ); w1 = w; }
void ExpandingLineElement::SetScale( float s ){ scale = s; }
void ExpandingLineElement::SetColor( float r, float g, float b ){ color.Set(r,g,b); }
void ExpandingLineElement::SetDropShadow( bool v, float offset, float depth ){ d_shadow = v; s_offset = offset; s_depth = depth; }

void ExpandingLineElement::Draw( ){
    render_buffer->LineWidth( 1.0f );

    std::vector<Vex2> pnts;

    float _t0 = (pos1.x-pos0.x-approach)/(pos1.x-pos0.x);

    Vex2 _pos0 = lerp( pos0, pos1, _t0 );

    pnts.push_back( Vex2(pos0.x,pos0.y+w0) );
    pnts.push_back( Vex2(pos0.x,pos0.y-w0) );

    for(int j = 0; j <= 20; j++){
        float t0 = (float)j/(float)20;
        float t1 = powf(t0,scale);

        Vex2 p = lerp( _pos0, pos1, t0 );
        float w = lerp( w0, w1, t1 );

        pnts.push_back( Vex2(p.x,p.y+w) );
        pnts.push_back( Vex2(p.x,p.y-w) );

    }

    render_buffer->LineWidth(1.0f);

    render_buffer->Color( color );
    render_buffer->Begin( render_buffer->QUAD_STRIP );
        for(int i = 0; i < (int)pnts.size(); i++){
            render_buffer->Vertex( pnts[i].x, pnts[i].y, depth );
        }
    render_buffer->End();

    render_buffer->Begin( render_buffer->LINE_LOOP);
        for(int i = 0; i < (int)pnts.size(); i+=2){
            render_buffer->Vertex( pnts[i].x, pnts[i].y, depth );
        }
        for(int i = (int)pnts.size()-1; i > 0; i-=2){
            render_buffer->Vertex( pnts[i].x, pnts[i].y, depth );
        }
    render_buffer->End();

    // Drop Shadow
    if( d_shadow ){
        render_buffer->PushMatrix();
            render_buffer->Translate( s_offset,-s_offset, s_depth );
            render_buffer->Color( 0.65f, 0.65f, 0.65f );
            render_buffer->Begin( render_buffer->QUAD_STRIP );
                for(int i = 0; i < (int)pnts.size(); i++){
                    render_buffer->Vertex( pnts[i].x, pnts[i].y, 0.0f );
                }
            render_buffer->End();

            render_buffer->Begin( render_buffer->LINE_LOOP);
                for(int i = 0; i < (int)pnts.size(); i+=2){
                    render_buffer->Vertex( pnts[i].x, pnts[i].y, 0.0f );
                }
                for(int i = (int)pnts.size()-1; i > 0; i-=2){
                    render_buffer->Vertex( pnts[i].x, pnts[i].y, 0.0f );
                }
            render_buffer->End();
        render_buffer->PopMatrix();
    }

}

float ExpandingLineElement::DistanceToObject( float x, float y ){
    return FLT_MAX;
}



LineElement::LineElement( RenderBuffer * rb ) : RenderableElement(rb), d_shadow(false) {
    width = 1;
}

void LineElement::SetStart( float sx, float sy ){ pos0.Set(sx,sy); }
void LineElement::SetEnd( float sx, float sy ){ pos1.Set(sx,sy); }
void LineElement::SetLineWidth( float s ){ width = s; }
void LineElement::SetColor( float r, float g, float b ){ color.Set(r,g,b); }
void LineElement::SetDropShadow( bool v, float offset, float depth ){ d_shadow = v; s_offset = offset; s_depth = depth; }

void LineElement::Draw( ){
    render_buffer->LineWidth(width);
    render_buffer->Color( color );
    render_buffer->Begin( render_buffer->LINE_LOOP);
        render_buffer->Vertex( pos0.x, pos0.y, depth );
        render_buffer->Vertex( pos1.x, pos1.y, depth );
    render_buffer->End();

    // Drow Shadow
    if( d_shadow ){
        render_buffer->PushMatrix();
            render_buffer->Translate( s_offset, -s_offset, s_depth );
            render_buffer->Color( 0.65f,0.65f,0.65f );
            render_buffer->Begin( render_buffer->LINE_LOOP);
                render_buffer->Vertex( pos0.x, pos0.y, 0.0f );
                render_buffer->Vertex( pos1.x, pos1.y, 0.0f );
            render_buffer->End();
        render_buffer->PopMatrix();
    }
}

float LineElement::DistanceToObject( float x, float y ){ return FLT_MAX; }



RenderCanvas::RenderCanvas( MyLib::RenderBuffer *rb ) : Renderable(rb) { }

void RenderCanvas::Clear( ){
    elemPlus.clear();
    elemStar.clear();
    elemExpLine.clear();
    elemLine.clear();
    elemRect.clear();
    elemText.clear();
    elemBinRect.clear();
}

void RenderCanvas::Draw( ){
    render_buffer->Enable( render_buffer->DEPTH_TEST );
        for(int i = 0; i < (int)elemPlus.size();    i++){ elemPlus[i].Draw();     }
        for(int i = 0; i < (int)elemStar.size();    i++){ elemStar[i].Draw();     }
        for(int i = 0; i < (int)elemExpLine.size(); i++){ elemExpLine[i].Draw();  }
        for(int i = 0; i < (int)elemLine.size();    i++){ elemLine[i].Draw();     }
        for(int i = 0; i < (int)elemRect.size();    i++){ elemRect[i].Draw();     }
        for(int i = 0; i < (int)elemBinRect.size(); i++){ elemBinRect[i].Draw();  }
        for(int i = 0; i < (int)elemText.size();    i++){ elemText[i].Draw();     }
    render_buffer->Disable( render_buffer->DEPTH_TEST );
}
*/

