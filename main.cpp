#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <X11/Xlib.h>
//#include <QAudioInput>

using namespace std;

Display *display;
Window window;
int screen;

XEvent e;

char font[2048];
char *textfile=NULL;
int startfile=0;
int key;
int len=20;
int step=50;
GC gc;
int maxx=1024;
int maxy=700;

int r=0,g=0,b=0;

unsigned long _RGB(int r,int g, int b)
{
    return b + (g<<8) + (r<<16);
}


struct SYMBOLSETTINGS
{
    int x,y,
    sizesymbol;//размер
    unsigned long
    colorback, //цвет заднего фона
    colorforeground;//цвет переднего фона
} symbol={0,0, 2, _RGB(0,0,255),_RGB(255,255,255)};
//symbol это структура содержащая информацию о печатание символа


int loadfont()
{
    ifstream ifl;
    ifl.open("res//font.dat",std::ios::binary | ifl.in);
    for(int i=0; i<sizeof(font); i++) font[i]=1+4+16+64;

    if (ifl.is_open())
    {
        ifl.read(font,sizeof(font));
        ifl.close();
        return 1;
    }
    else return 0;
}

int loadtext()
{
    ifstream ifl;
    ifl.open("res//newz.txt",std::ios::binary | ifl.in);

    if (ifl.is_open())
    {
        ifl.seekg(0, std::ios::end);
        int size=ifl.tellg();
        ifl.seekg(0, std::ios::beg);
        textfile=new char[size+1];

        ifl.read(textfile,size);
        ifl.close();
        return 1;
    }
    else return 0;
}

void rect(int x, int y, int len, int hei, int color)
{
    XSetForeground(display, gc, color);
    XFillRectangle( display, window, gc, x, y, len, hei );
}


void drawsymbol(int xc, int yc, int sizesymbol, unsigned char n, int colorback, int colorforeground)
{
    int j=n*8;
    for(int y=0; y<8;y++)
    {
        unsigned char sym=font[j];
        unsigned char mask=128;

        for(int x=0; x<8;x++)
        {
            int c;
            if(sym&mask)
            {
                c=colorforeground;
            }
            else c=colorback;

            rect(xc+x*sizesymbol, yc+y*sizesymbol, sizesymbol, sizesymbol, c);

            mask>>=1;

        }

        j++;
    }
}

void drawstring(int x, int y, char * str)
{
    int len=symbol.sizesymbol*8;
    symbol.x=x;
    symbol.y=y;

    for(int i=0; i<strlen(str); i++)
    {
        unsigned char sym=str[i];

        if (sym==32)
        {
            //symbol.colorforeground=_RGB(rand(),rand(),rand());
        }

        //if (sym>=32)
        {
            int clr;
            if (sym==13) clr=_RGB(0,255,0);
            else if (sym==10) clr=_RGB(0,0,255);
            else clr=symbol.colorforeground;
            drawsymbol(symbol.x, symbol.y, symbol.sizesymbol, sym, symbol.colorback, clr);
        }
        symbol.x+=len;

       if ( (symbol.x>(maxx-len-1)) || (sym==10))
        {
            symbol.x=0;
            symbol.y+=len;

            if (symbol.y> (maxy-len)) return;
        }

    }

}

void draw()
{
    for(int x=0; x<maxx; x+=step)
    {
        for(int y=0; y<maxy; y+=step)
        {
            rect(x, y, len, len,_RGB(r,g,b) );
        }
     }

}

void ReDraw()
{
    rect(0, 0, maxx, maxy, symbol.colorback );
    drawstring(0,40, textfile+startfile);

}


void printstr(int x, int y, char *str)
{
    rect(x,y-20,100,30,_RGB(255,255,255));
    XSetForeground(display, gc, _RGB(0,100,100));
    XDrawString( display, window,  gc, x, y, str, strlen( str ) );
}


int main( void )
{

   if( ( display = XOpenDisplay( getenv("DISPLAY" ) ) ) == NULL ) {  // Соединиться с X сервером,
      printf( "Can't connect X server: %s\n", strerror( errno ) );
      exit( 1 );
   }

   cout<<"X server return "<<display<<endl;

   screen = DefaultScreen( display );

   int delta=0;
   window = XCreateSimpleWindow( display, RootWindow( display, screen ),     // Создать окно
                            delta, delta, maxx-delta, maxy-delta, 1,
                                _RGB(255,255,255), _RGB(0,0,255));
//                            BlackPixel( display, screen ), WhitePixel( display, screen ) );

   XSelectInput( display, window, ExposureMask | KeyPressMask );  // На какие события будем реагировать?
   XMapWindow( display, window );

   gc=DefaultGC( display, screen );

   loadfont();
   loadtext();

   //int sec=GetTickCount();

   time_t seconds = time(NULL);
   tm* timeinfo = localtime(&seconds);
   srand(seconds);

   //int nomer=loadpicture("vandam.jpg");

   startfile=rand()%695000;

   while(1)
   {                      // Бесконечный цикл обработки событий
      XNextEvent( display, &e );
      if( e.type == Expose )
      {
                           // Перерисовать окно
        ReDraw();
      }

      key=e.xkey.keycode;

      if( (e.type == KeyPress) && (key==36 || key==9) )    break;  // При нажатии кнопки - выход


      if (key==46) // KEY_L
      {
        ifstream f;
        //int ff=
        f.open("res//1.txt", f.in);
        bool is_open=f.is_open();
        if (is_open)
        {
            char buf[255];
            //int size=
            f.getline(buf,255);

            printstr(0,120,buf);

            //awstring(0,90,buf);
            drawstring(0,0, textfile);
        }


        f.close();
      }

      if (key==114)
      {
            len++;
            r+=25;
            draw();
      }

      if (key==113 && len>2)
      {
            len--;
            g+=30;
            draw();
      }

      if(key==114)
      {
        if(symbol.sizesymbol<10) symbol.sizesymbol++;
        ReDraw();
      }

      if(key==113)
      {
        if(symbol.sizesymbol>1) symbol.sizesymbol--;
        ReDraw();
      }


      //стрелка
      if (key==116)
      {
        do {
        char k=textfile[startfile];

        if ((k!=13)||(k!=10)) startfile++;
        if (k==10) break;


        } while(1);

        ReDraw();

      }

    if (key==65)
    {
        startfile=rand()%695000;
        ReDraw();
    }

      //стрелка
      if (key==111)
      {
        startfile-=2;
        do {
            if (startfile<0) { startfile=0; break;}

            char k=textfile[startfile];
            //if (k>=32)
            startfile--;
            if (k==10)
            {
                startfile+=2;
                if (startfile<0) startfile=0;
                break;
            }

        } while(1);

        ReDraw();

      }




      char msg[64];// = "Key code=%i";
      sprintf(msg, "Key code=%i", key);

      static int jtemp=0;
      jtemp+=10;
      if (jtemp>maxy) jtemp=0;

      XSetForeground(display, gc, _RGB(0,100,100));
      XDrawString( display, window,  gc, 0, 10+jtemp, msg, strlen( msg ) );


    }

    cout<<"e.xkey="<<key<<endl;
    //getch();

   XCloseDisplay( display );                                 // Закрыть соединение с X сервером

   return 0;
}


