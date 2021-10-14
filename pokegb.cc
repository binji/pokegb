#include                                                                                                      <SDL2/SDL.h>  /******************************************/
#include                                                                                                      <sys/mman.h>  /*     POKEGB by Ben Smith (June 2021)    */
#include                                                                                                        <unistd.h>  /* -------------------------------------- */
#include                                                                                                         <cstdint>  /* A GB emulator that can only play       */
#include                                                                                                         <fcntl.h>  /* Pokemon Blue/Red, written in C++.      */
#define P                                                                                                             case  /* Requires gcc/clang and Linux/macOS.    */
#define O                                                                                                             goto  /* Many features are not implemented!     */
#define K                                                                                                           break;  /*                                        */
#define E                                                                                                           return  /* $ cc pokegb.cc -lSDL2 -o pokegb        */
#define M(_)                                                                                             L(_,2)f=(b-_)/16;  /* $ ./pokegb   # reads from rom.gb       */
#define o(_)                                                                                       P _..._+7:a=q(0,0,f=b);  /*              # writes to rom.sav       */
#define N(_)                                                                                   o(_)P _+70:u=b&64?i(k++):a;  /*                                        */
#define B(_)                                                                                  P _..._+55:o(_+56)f=(b-_)/8;  /* Controls: Up/Down/Left/Right=Arrow Keys*/
#define L(_,__)                                                                          P _:P _+8*__:P _+16*__:P _+24*__:  /*           B Button=Z, A Button=X       */
#define U(_)                                                                            L(_,1)L(_+32,1)a=q(0,0,f=(b-_)/8);  /*           Start=Enter, Select=Tab      */
#define e(_,__)                                                              L(_,1)P __:f=(b-_)/8,d=b==__||!(F&aj[f])^f&1;  /******************************************/
                     using g=uint8_t                                         ;g b,f,a,u,d,m                                          ,*S,*T,h[512],
                Z[8192],ac[16384],*ad,*C                                ,r[]={19,0,216,0,77,1,176                               ,1},&F=r[6],&j=r[7],*ae[
             ]={r+1,r,r+3,r+2,r+5,r+4,&F,&j                          },&H=h[271],&z=h[320],&s=h[324                          ],A,V,*v;using x=uint16_t;x k=
           256,*y=(x*)r,&n=y[2],w=65534,&af=(                      x&)h[259],W=32,*X[]={y,y+1,&n,&w},                      *ai[]={y,y+1,&n,&n},Y,I;using p=int
         ;p c,aj[]={128,128,16,16},ag[23040],ak                  []={-1,-23197,-65536,-1<<24,-1,-8092417                 ,-12961132,-1<<24};g i(x a=n,g b=0,p c
       =0){I+=4;switch(a>>13){P 1:if(c)T=S+((b?b&              63:1)<<14);P 0:E S[a];P 2:if(c&&b<=3)C=ad+              (b<<13);P 3:E T[a&16383];P 4:a&=8191;if(c)
      Z[a]=b;E Z[a];P 5:a&=8191;if(c)C[a]=b;E C[a]            ;P 7:if(a>=65024){if(c){if(a==65350)for(p y=            160;--y>=0;)h[y]=i(b<<8|y);h[a&511]=b;}if(a
     ==65280){if(~h[256]&16)E~(16+v[81]*8+v[82]*4+v          [80]*2+v[79]);if(~h[256]&32)E~(32+v[40]*8+v[43          ]*4+v[29]*2+v[27]);E 255;}E h[a&511];}P 6:a&=
    16383;if(c)ac[a]=b;E ac[a];}}g D(g a,p b,p c,p d        ,p R){E F=F&a|!b<<7|c<<6|d<<5|R<<4;}x J(x&b=k){a        =i(b++);E i(b++)<<8|a;}g t(x a){i(--w,a>>8,1);i(
   --w,a,1);E I+=4;}g q(g a,p b=1,g c=f){E(c&=7)==6?i      (n,a,b):b?*ae[c]=a:*ae[c];}g ah(p a,p b,p c){g*d=&      Z[a*16+b*2];E(d[1]>>c)%2*2+(*d>>c)%2;}p main(){T=(
  S=(g*)mmap(0,1<<20,1,1,open("rom.gb",0),0))+32768;c=    open("rom.sav",66,438);ftruncate(c,32768);SDL_Init(     32+0+0);auto*aa=SDL_CreateRenderer(SDL_CreateWindow(
 "pokegb",0,0,800,720,4),-1,4);C=ad=(g*)mmap(0,32768,3,  1,c,0);v=(g*)SDL_GetKeyboardState(0);z=145;af=44032;    auto*ab=SDL_CreateTexture(aa,376840196,1,160,144);for(
 ;;){Y=I;if(A&H&h[511])H=V=A=0,I+=8,t(k),k=64;else if(V  )I+=4;else switch(b=i(k++)){M(1)*X[f]=J();P 0:K M(10)M  (2)c=b&8;q(i(*ai[f],j,!c),c,7);n+=f<2?0:5-2*f;K M(11)M
(3)*X[f]+=b&8?-1:1;I+=4;        K U(5)U(4)m=b&1;q(a+=1-m*2);D(16,a,m,!(a+m&15),0        );K U(6)q(i(k++));K M(9)c=*X[f];D(128,1,0,n%4096        +c%4096>4095,n+c>65535);
n+=c;I+=4;K L(7,1)m=1              ;O J;e(32,24)a=i(k++);if(d)k+=(int8_t)a,I+=             4;K P 39:d=a=0;if(F&32||~F&64&&j%16>9)a=6;              if(F&16||~F&64&&j>153
)a|=96,d=1;D(65,j+=F&              64?-a:a,0,0,d);K P 47:j=~j;D(144,1,1,1,0);              K P 55:P 63:D(128,1,0,0,b&8?!(F&16):1);K B              (64)b==118?V=1:q(a);K
N(128)m=d=0;O F;N(136              )m=0;d=F/16%2;O F;N(184)O e;N(144)e:d=1;O               B;N(152)d=!(F/16%2);B:m=1;u=~u;F:D(0,a=j+u              +d,m,(j%16+u%16+d>15)
^m,(j+u+d>255)^m);if(              ~(b/8)&7)j=a;K N(160)D(0,j&=u,0,1,0);K N(               168)D(0,j^=u,0,0,0);K N(176)D(0,j|=u,0,0,0             );K P 217:d=A=1;O Z;e(
192,201)           Z:I            +=4           ;if(d)k=J(w);K M(          193            )y[           f]=J(w);K e(194,           195           )O C            ;e(196,
205)C:c=           J()            ;if           (d)b&4?t(k),0:I+=          4,k            =c;           K M(197)t(y[f]);           K P            203          :m=0;b=i(
k++);J:;;          switch       (b){           o(0)o(16)o(32)d=a>>          7;a+=       a+(b           &16?F/16%2:b&32?0:           d);O        I;o(           48)d=0;a=
 a*16+a/             16;O I;o(8)o(24           )o(40)o(  56)d=a&1            ;a=(b&48)==32?            (int8_t)  a>>1:a/2            +(b&32?0:b&16?            (F*8&128
 ):d*128);              I:q(a);D              (0,m||a,0  ,0,d);K B              (64)D(16              ,a&1<<f,0  ,1,0);K B                                    (128)q(a&
  ~(1<<f),1                                  ,b);K B(     192)q(a|1                                  <<f,1,b);    }K P 224:                                 P 226:P 234
   :P 240:P                                 242:P 250      :c=b&16;q                                (i(b&8?J(      ):65280+(                                b&2?*r:i(
    k++)),j,!c                            ),c,7);K P        233:k=n;K                             P 243:P 251       :A=b!=243;                            K P 248:n=
     w+(int8_t)(                        a=i(k++));D          (0,1,0,w%16                        +a%16>15,(g          )w+a>255);I                        +=4;K P 249
     :w=n;I+=4;}for                   (af+=I-Y;Y++            !=I;)if(z&128                  ){if(++W==456)           {if(s<144)for                   (c=160;--c>=
       0;){g b=z&32&&s            >=h[330]&&c>=h[              331]-7,d=b?c-h[            331]+7:c+h[323]              ,R=b?s-h[330]:s            +h[322];x f=0,i
         =Z[(z&(b?64:8)?7:6)<<10|R/8*32+d/8],j=                  ah(z&16?i:256+(int8_t)i,R&7,7-d&7);if(                  z&2)for(g*a=h;a<h+160;a+=4){g k=c-a[1]
           +8,l=s-*a+16,m=ah(a[2],l^(a[3]&64?                      7:0),k^(a[3]&32?0:7));if(k<8&&l<8&&                     !(a[3]&128&&j)&&m){j=m;f=1+!!(a[3]
            &16);K}}ag[s*160+c]=ak[(h[327+f                          ]>>2*j)%4+f*4&7];}if(s==143){H                         |=1;SDL_Event b;SDL_UpdateTexture
              (ab,0,ag,640);SDL_RenderCopy                            (aa,ab,0,0);SDL_RenderPresent                            (aa);for(;SDL_PollEvent(&b
                   );)if(b.type==256                                       )E 0;}s=(s+1)%154;                                      W=0;}}else s=W=0;}}
