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

#ifndef SCI_GRAPHICS_RENDERABLEELEMENTS_H
#define SCI_GRAPHICS_RENDERABLEELEMENTS_H

#include <math.h>
#include <algorithm>
#include <string>

#include <SCI/Vex3.h>

#include <GL/oglFont.h>

namespace SCI {
    namespace Graphics {

        class RenderElement {

        public:
            RenderElement( );
            virtual ~RenderElement();

            virtual float DistanceToObject( float x, float y ) = 0;
            virtual void Draw( ) = 0;

            void SetTooltip( const char * str );
            void SetTooltip( std::string  str );
            std::string GetTooltip();

            void SetDepth( float d );

            void   SetUserData( void * v );
            void * GetUserData( );

        protected:
            void * user_data;
            std::string tooltip;

            float depth;

        };

        class RectangularRenderElement : public RenderElement {

        public:
            RectangularRenderElement( );

            void SetSize( float sx, float sy );
            void SetSize( float s );
            void SetCenter( float cx, float cy );
            void SetOrientation( float _angle );

            virtual float DistanceToObject( float x, float y );

            void SetSolid( bool v, SCI::Vex3 col );
            void SetSolid( bool v, float r = 0, float g = 0, float b = 0 );

            void SetEmpty( bool v, SCI::Vex3 col );
            void SetEmpty( bool v, float r = 0, float g = 0, float b = 0 );

            void SetDropShadow( bool v, float offset, float depth  );

            void SetLineWidth( float val );

        protected:
            float s_offset, s_depth;
            int type;
            float line;
            SCI::Vex3 s_color;
            SCI::Vex3 e_color;
            SCI::Vex2 center;
            SCI::Vex2 size;
            float angle;

        };


        class TextElement : public RenderElement {
        public:

            TextElement( oglWidgets::oglFont * font );

            void SetSize( float s );
            void SetOrientation( float _angle );

            void SetRight( float rx );
            void SetLeft( float lx );
            void SetCenter( float cx );

            void SetTop( float ty );
            void SetMiddle( float my );
            void SetBottom( float by );

            void SetAspect( float w_div_h );


            void SetText( const char * _txt );
            void SetTextf(const char *fmt, ...);

            virtual void Draw( );

            virtual float DistanceToObject( float x, float y );

            void SetColor( float r = 0, float g = 0, float b = 0 );

            void SetDropShadow( bool v, float offset, float depth  );

        protected:
            std::string txt;

            bool d_shadow;
            float s_offset, s_depth;
            int j_type;
            SCI::Vex3 color;
            SCI::Vex2 center;
            SCI::Vex2 size;
            float angle;

            float aspect;

            oglWidgets::oglFont * font;

        };

        /*
        class BinnedRectangularElement : public RenderableRectangularElement {
        public:
            BinnedRectangularElement( RenderBuffer * rb );

            void SetBinCount( int cnt );
            void SetBinColor( int bin, float r = 0, float g = 0, float b = 0 );
            void SetBinColor( int bin, SCI::Vex3 col );

            virtual void Draw( );

        protected:
            std::vector< SCI::Vex3 > bcols;


        };

        class ExpandingLineElement : public RenderableElement {

        public:
            ExpandingLineElement( RenderBuffer * rb );

            void SetStart( float sx0, float sy0, float w0 );
            void SetEnd( float sx1, float sy1, float w1 );
            void SetScale( float s );
            void SetApproach( float a );

            void SetColor( float r = 0, float g = 0, float b = 0 );
            void SetDropShadow( bool v, float offset, float depth );

            virtual void Draw( );
            virtual float DistanceToObject( float x, float y );

        protected:
            bool d_shadow;
            float s_offset, s_depth;
            SCI::Vex3 color;
            SCI::Vex2 pos0, pos1;
            float w0,w1;
            float scale;
            float approach;

        };


        class LineElement : public RenderableElement {

        public:
            LineElement( RenderBuffer * rb );

            void SetStart( float sx, float sy );
            void SetEnd( float sx, float sy );
            void SetLineWidth( float s );

            void SetColor( float r = 0, float g = 0, float b = 0 );
            void SetDropShadow( bool v, float offset, float depth );

            virtual void Draw( );
            virtual float DistanceToObject( float x, float y );

        protected:
            bool d_shadow;
            float s_offset, s_depth;
            SCI::Vex3 color;
            SCI::Vex2 pos0, pos1;
            float width;

        };





        class RectElement : public RenderableRectangularElement {
        public:

            RectElement( MyLib::RenderBuffer * rb );

            virtual void Draw( );

        };

        class PlusElement : public RenderableRectangularElement {
        public:

            PlusElement( MyLib::RenderBuffer * rb );

            virtual void Draw( );

        };

        class StarElement : public RenderableRectangularElement {
        public:

            StarElement( MyLib::RenderBuffer * rb );

            void Draw( );
        };


        class RenderCanvas : public Renderable {

        public:
            RenderCanvas( MyLib::RenderBuffer *rb );

            virtual void Clear( );
            virtual void Draw( );

        protected:
            std::vector<MyLib::PlusElement> elemPlus;
            std::vector<MyLib::StarElement> elemStar;
            std::vector<MyLib::RectElement> elemRect;
            std::vector<MyLib::ExpandingLineElement> elemExpLine;
            std::vector<MyLib::LineElement> elemLine;
            std::vector<MyLib::TextElement> elemText;
            std::vector<MyLib::BinnedRectangularElement> elemBinRect;
        };
    }
    */
    }
}

#endif // SCI_GRAPHICS_RENDERABLEELEMENTS_H
