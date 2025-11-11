
import javax.swing.*;
import java.awt.event.*;
import java.awt.*;
import java.awt.geom.*;
import java.io.*;
import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
class Ball extends Ellipse2D.Float
{
    Board p;
    float dx,dy,speed;
    int bfallen, breleased, playerbounced, itype;
    Ball(Board p,float x,float y,float dx,float dy)
    {
        this.x=x;
        this.y=y;
        this.width=10;
        this.height=10;
        this.speed=1;
        this.p=p;
        this.dx=dx;
        this.dy=dy;
        this.bfallen=0;
        this.breleased=1;
        this.playerbounced=1;
        this.itype=0;
    }

    void gotbonus()
    {
        if (p.ibonus==0)//speed
        {
            float temp=(float)(Math.sqrt(dx*dx+dy*dy));
            speed=100;
            dx=(float)Math.sqrt(18)*2/temp*dx;
            dy=(float)Math.sqrt(18)*2/temp*dy;
        }
        else if (p.ibonus==1)//slow ball
        {
            float temp=(float)(Math.sqrt(dx*dx+dy*dy));
            dx=dx/(1+speed/100);
            dy=dy/(1+speed/100);
            speed=1;
        }
        else if (p.ibonus==2)//expand paddle
        {
            if (p.obonus==1)
            {
                if (p.b1.width<150)
                {
                    p.b1.width+=30;
                    if (p.b1.x+p.b1.width>390)
                    {
                        p.b1.x=390-p.b1.width-1;
                    }
                    else
                    {
                        p.b1.x+=15;
                    }
                }
            }
            else
            {
                if (p.b2.width<150)
                {
                    p.b2.width+=30;
                    if (p.b2.x+p.b2.width>390)
                    {
                        p.b2.x=390-p.b2.width-1;
                    }
                    else
                    {
                        p.b2.x+=15;
                    }
                }
            }

        }
        else if (p.ibonus==3)//shorten paddle
        {
            if (p.obonus==1)
            {
                if (p.b1.width>60)
                {
                    p.b1.width-=30;
                    p.b1.x+=15;
                }
            }
            else
            {
                if (p.b2.width>60)
                {
                    p.b2.width-=30;
                    p.b2.x+=15;
                }
            }
        }
        else if (p.ibonus==4)//fireball
        {
            if (p.a.itype==0) p.a.itype=2;
            else if (p.a.itype==1) p.a.itype=3;
        }
        else if (p.ibonus==5)//brick-piercing ball
        {
            if (p.a.itype==0) p.a.itype=1;
            else if (p.a.itype==2) p.a.itype=3;
        }
        else if (p.ibonus==6)//extra points
        {
            if(p.obonus==1) p.points1+=4;
            else p.points2+=4;
        }
        else if (p.ibonus==7)//magnet
        {
            if(p.obonus==1) p.b1.bmagnet=true;
            else p.b2.bmagnet=true;
        }
        else if (p.ibonus==8)//remove any bonus
        {
            itype=0;
            if(p.obonus==1) p.b1.bmagnet=false;
            else p.b2.bmagnet=false;
        }
    }

    void flametick()//gradual burning
    {
        for (int i=0;i<p.lb[p.level];i++)
        {
            if(p.ld[i]==2 || p.ld[i]==12)
            {
                if(p.ld[i]==2) p.points1++;
                else p.points2++;
                p.ld[i]=1;
                if (p.ltt[i]>0)p.remaining--;
                createbonus(i);
            }
            if (p.ld[i]>2)
            {
                p.ld[i]--;
            }
        }
    }

    void createbonus(int i)//a change to generate bonus
    {
        if (p.bbonus==false && p.remaining>0)
        {
            if(Math.random()*100<25)
            {
                p.bbonus=true;
                p.obonus=playerbounced;
                p.ibonus=(int)(Math.random()*90)%9;
                p.xbonus=p.lx[p.level][i]+5;
                p.ybonus=p.ly[p.level][i];
            }
        }
    }

    void nextKrok()
    {
        if (p.bpaused==false)
        {
            if (p.bbonus==true)//move bonus icon
            {
                if (p.obonus==1) p.ybonus+=3;
                else p.ybonus-=3;
                if (p.ybonus+20>p.b1.y && p.ybonus<p.b1.y+p.b1.height && p.xbonus+20>p.b1.x && p.xbonus<p.b1.x+p.b1.width)
                {
                    gotbonus();
                    p.points1++;
                    p.bbonus=false;
                }
                if (p.ybonus+20>p.b2.y && p.ybonus<p.b2.y+p.b2.height && p.xbonus+20>p.b2.x && p.xbonus<p.b2.x+p.b2.width)
                {
                    gotbonus();
                    p.points2++;
                    p.bbonus=false;
                }
                if (p.ybonus>370 || p.ybonus<0)
                {
                    p.bbonus=false;
                }
            }
            if (breleased==0)//move ball
            {
                x+=dx;
                y+=dy;
                if(getMinX()<0)
                {
                    x=1;
                    dx=-dx;
                    speed++;
                }
                if(getMaxX()>390)
                {
                    x=390-width;
                    dx=-dx;
                    speed++;
                }
                if(getMinY()<0)//ball lost
                {
                    bfallen=2;
                }
                if(getMaxY()>370)
                {
                    bfallen=1;
                }
                if(this.y >=p.b1.y-5 && this.y <=p.b1.y+p.b1.height && this.x+this.width>=p.b1.x && this.x<=p.b1.x+p.b1.width && dy>0)
                {//ball hits paddle1
                    playerbounced=1;
                    if (p.b1.bmagnet==true) breleased=1;
                    speed++;
                    if (speed>100) speed=100;
                    float dl =(float)Math.sqrt(18)*(1+speed/100);
                    dy=-dl*(1f - Math.abs(this.x+this.width/2-p.b1.x-p.b1.width/2)/p.b1.width*2*5/6);
                    dx=Math.signum(-p.b1.x-p.b1.width/2+this.x+this.width/2)*(float)Math.sqrt(dl*dl-dy*dy);
                }
                if (this.y>=p.b2.y && this.y<=p.b2.y+p.b2.height+5 && this.x+this.width>=p.b2.x && this.x<=p.b2.x+p.b2.width && dy<0)
                {//ball hits paddle2
                    playerbounced=2;
                    if (p.b2.bmagnet==true && p.secplayer==true) breleased=2;
                    speed++;
                    if (speed>100) speed=100;
                    float dl =(float)Math.sqrt(18)*(1+speed/100);
                    dy=dl*(1f - Math.abs(this.x+this.width/2-p.b2.x-p.b2.width/2)/p.b2.width*2*5/6);
                    dx=Math.signum(-p.b2.x-p.b2.width/2+this.x+this.width/2)*(float)Math.sqrt(dl*dl-dy*dy);
                }
                for (int i=0;i<p.lb[p.level];i++)//collision detection
                {
                    if (p.ld[i]!=0) continue;
                    if (p.l[i].intersects(p.a.getBounds2D()))
                    {
                        if (itype==2)
                        {
                            if (playerbounced==1) p.ld[i]=10;
                            else p.ld[i]=20;
                        }
                        else if (p.ltt[i]>2 || itype==1 || itype==3)
                        {
                            p.ld[i]=1;
                            if (playerbounced==1) p.points1++;
                            else p.points2++;
                            if (p.ltt[i]!=0) p.remaining--;
                            createbonus(i);
                        }
                        else if (p.ltt[i]==1 || p.ltt[i]==2)
                        {
                            p.ltt[i]++;
                        }
                        if (itype!=1 && itype!=3)//ball not brick-piercing, must bounce
                        {
                            float hx = Math.abs(p.a.x+p.a.width/2-p.l[i].x-p.l[i].width/2) -
                                    p.l[i].width/2;
                            float hy = Math.abs(p.a.y+p.a.height/2-p.l[i].y-p.l[i].height/2) -
                                    p.l[i].height/2;
                            if (hx < hy)
                            {
                                dy=-dy;
                            }
                            else
                            {
                                dx=-dx;
                            }
                        }
                        if (p.a.itype>1)//fireball effect
                        {
                            for (int j=0;j<p.lb[p.level];j++)
                            {
                                if (p.ld[j]==0 && p.l[i].intersects(new Rectangle2D.Float(p.lx[p.level][j]-15,p.ly[p.level][j]-5,60,20)))
                                {
                                    if (playerbounced==1) p.ld[j]=10;
                                    else p.ld[j]=20;
                                }
                            }
                        }
                        speed++;
                        break;
                    }
                }
            }
            flametick();
            if (p.secplayer==false)//computer following
            {
                if (x+width<p.b2.x+p.b2.width/4 && p.b2.x>5)
                {
                    p.b2.x-=5;
                }
                else if (x>p.b2.x+p.b2.width*3/4 && p.b2.x+p.b2.width<385)
                {
                    p.b2.x+=5;
                }
            }
        }
        p.repaint();
        Toolkit.getDefaultToolkit().sync();
    }
}
class Brick extends Rectangle2D.Float
{
    Brick(int x, int y)
    {
        this.width=30;
        this.height=10;
        this.x=x;
        this.y=y;
    }
}
class Engine extends Thread
{
    Ball a;
    Engine(Ball a)
    {
        this.a=a;
        start();
    }
    public void run()
    {
        try
        {
            while(true)

            {
                if (a.p.remaining==0)//all bricks destroyed
                {
                    sleep(500);
                    if (a.p.level==2)//last level cleared, stop ball
                    {
                        a.dx=0;
                        a.dy=0;
                        a.p.remaining--;
                    }
                    else
                    {
                        a.p.level++;
                        a.p.setlevel();
                    }
                }
                if (a.bfallen==1)//player1 let ball fall
                {
                    a.p.points1--;
                    sleep(500);
                    a.speed=1;
                    a.width=10;
                    a.height=10;
                    a.p.b1.width=90;
                    a.x=a.p.b2.x + a.p.b2.width*3/4;
                    a.y=a.p.b2.y+10;
                    a.itype=0;
                    a.p.b1.bmagnet=false;
                    a.dx=3;
                    a.dy=3;
                    if (a.p.secplayer==true) a.breleased=2;
                    else a.breleased=0;
                    a.playerbounced=2;
                    a.bfallen=0;
                }
                if (a.bfallen==2)//player2 let ball fall
                {
                    a.p.points2--;
                    sleep(500);
                    a.speed=1;
                    a.width=10;
                    a.height=10;
                    a.p.b2.width=90;
                    a.x=a.p.b1.x + a.p.b1.width*3/4;
                    a.y=a.p.b1.y - a.p.b1.height;
                    a.itype=0;
                    a.p.b2.bmagnet=false;
                    a.dx=3;
                    a.dy=-3;
                    a.breleased=1;
                    a.playerbounced=1;
                    a.bfallen=0;
                }
                a.nextKrok();
                sleep(20);
            }
        }
        catch(InterruptedException e){}
    }
}
class Paddle extends Rectangle2D.Float
{
    boolean bmagnet;

    Paddle(int x, int y)
    {
        this.x=x;
        this.y=y;
        this.width=90;
        this.height=10;
        this.bmagnet=false;
    }
    void setX(int x)
    {
        this.x=x;
    }
}
class Board extends JPanel implements MouseMotionListener, MouseListener, KeyListener
{
    Paddle b1,b2;
    Ball a;
    Engine s;
    Brick l[];
    int points1, points2, remaining, level, ibonus,obonus;
    int ld[], lb[], ltt[];
    int[][] lx,ly,lt;
    float xbonus,ybonus;
    BufferedImage[] btxt, batxt, ptxt, brtxt;
    boolean secplayer,bpaused, bcomp, bbonus;
    Board(int[][] lx, int[][] ly, int[] lb, int[][] lt)
    {
        super();
        addMouseMotionListener(this);
        addMouseListener(this);
        addKeyListener(this);
        setFocusable(true);
        requestFocusInWindow();
        this.l=l;
        ld=new int[100];
        this.lb=lb;
        this.lx=lx;
        this.ly=ly;
        this.lt=lt;
        level=0;
        this.ltt=new int[lb[level]];
        for (int i=0; i<lb[level];i++)
        {
            ltt[i]=lt[level][i];
        }
        points1=0;
        points2=0;
        bcomp=true;
        bbonus=false;
        ibonus=0;
        obonus=0;
        secplayer=false;
        bpaused=false;
        remaining=lb[level];
        b1=new Paddle(100,350);
        b2=new Paddle(100,10);
        a=new Ball(this,150,340,3,-3);
        s=new Engine(a);

        btxt = new BufferedImage[9];
        ptxt = new BufferedImage[4];
        brtxt = new BufferedImage[11];
        String[] plik = {"bonus5.bmp","bonus6.bmp","bonus1.bmp","bonus2.bmp","bonus3.bmp","bonus4.bmp","bonus7.bmp","bonus8.bmp","bonus9.bmp"};
        String[] plik2 = {"bpaddle60.bmp","bpaddle90.bmp","bpaddle120.bmp","bpaddle150.bmp"};
        String[] plik3 = {"bhard.bmp","bgray3.bmp","bgray2.bmp","bgray1.bmp","bblue.bmp","bgreencyan.bmp","borangeyellow.bmp","bbluepurple.bmp","bflame1.bmp","bflame2.bmp","bflame3.bmp"};
        for (int i=0;i<11;i++)//take textures from files
        {
            try
            {
                File f = new File(plik3[i]);
                brtxt[i] = ImageIO.read(f);
                if (i<4)
                {
                    f = new File(plik2[i]);
                    ptxt[i] = ImageIO.read(f);
                }
                if (i<9)
                {
                    f = new File(plik[i]);
                    btxt[i] = ImageIO.read(f);
                }

            }
            catch(IOException e)
            {
                System.err.println("Problem z plikiem");
            }
        }
        setlevel();

    }
    void setlevel()//prepare next level, remove any bonus from players
    {
        a.dx=3;
        a.width=10;
        a.height=10;
        a.p.b1.width=90;
        a.p.b2.width=90;
        if (a.playerbounced==1)
        {
            a.x=a.p.b1.x + a.p.b1.width*3/4;
            a.y=a.p.b1.y-10;
            a.dy=-3;
        }
        else
        {
            a.x=a.p.b2.x + a.p.b2.width*3/4;
            a.y=a.p.b2.y + a.p.b2.height+10;
            a.dy=3;
        }
        a.itype=0;
        a.p.b1.bmagnet=false;
        a.p.b2.bmagnet=false;
        if (a.playerbounced==1) a.breleased=1;
        else if (a.p.secplayer==true) a.breleased=2;
        else a.breleased=0;
        a.speed=1;
        bbonus=false;
        remaining=0;
        for (int i=0;i<lb[level];i++)
        {
            if (lt[level][i]>0) remaining++;
        }
        ltt=new int[lb[level]];
        for (int i=0; i<lb[level];i++)
        {
            ltt[i]=lt[level][i];
        }
        l=new Brick[100];
        for (int i=0; i<lb[level]; i++)
        {
            l[i]=new Brick(lx[level][i],ly[level][i]);
            ld[i]=0;
        }
    }

    public void paintComponent(Graphics g)
    {
        super.paintComponent(g);
        Graphics2D g2d=(Graphics2D)g;
        Shape r=new Rectangle2D.Float(0,0,390,370);
        if (level==0) g2d.setPaint(new RadialGradientPaint(200,100+(-remaining+lb[level])*2,500,new float[]{0.2f,1.0f},new Color[]{Color.gray,Color.black}));
        else if (level==1) g2d.setPaint(new RadialGradientPaint(150+(float)Math.sin((-remaining+lb[level])/15)*50,150+((float)Math.cos((-remaining+lb[level])/15))*50,500,new float[]{0.2f,1.0f},new Color[]{Color.getHSBColor(0.7f,0.6f,1),Color.getHSBColor(0.7f, 1, 1)}));
        else g2d.setPaint(new RadialGradientPaint(300,200,500,new float[]{0.2f,1.0f},new Color[]{Color.gray,Color.black}));
        g2d.fill(r);
        if (b1.width<90) g2d.drawImage(ptxt[0],null,(int)b1.x,(int)b1.y);
        else if (b1.width<120) g2d.drawImage(ptxt[1],null,(int)b1.x,(int)b1.y);
        else if (b1.width<150) g2d.drawImage(ptxt[2],null,(int)b1.x,(int)b1.y);
        else g2d.drawImage(ptxt[3],null,(int)b1.x,(int)b1.y);
        if (b2.width<90) g2d.drawImage(ptxt[0],null,(int)b2.x,(int)b2.y);
        else if (b2.width<120) g2d.drawImage(ptxt[1],null,(int)b2.x,(int)b2.y);
        else if (b2.width<150) g2d.drawImage(ptxt[2],null,(int)b2.x,(int)b2.y);
        else g2d.drawImage(ptxt[3],null,(int)b2.x,(int)b2.y);

        if (a.itype==1) g2d.setPaint(new RadialGradientPaint(a.x+a.dx/2,a.y+a.dy/2,a.width,new float[]{0.2f,1.0f},new Color[]{Color.getHSBColor(0.7f,0.6f,1),Color.getHSBColor(0.7f,1,1)})); //g2d.setColor(java.awt.Color.blue);
        else if (a.itype>1) g2d.setPaint(new RadialGradientPaint(a.x+a.dx/2,a.y+a.dy/2,a.width,new float[]{0.2f,1.0f},new Color[]{Color.getHSBColor(0,0.7f,1),Color.getHSBColor(0,1,1)})); //g2d.setColor(java.awt.Color.red);
        else g2d.setPaint(new RadialGradientPaint(a.x+a.width/2+a.dx/2,a.y+a.width/2+a.dy/2,a.width,new float[]{0.2f,1.0f},new Color[]{Color.getHSBColor(0,0,0.75f),Color.getHSBColor(0,0,0.5f)}));
        g2d.fill(a);
        for (int i=0;i<lb[level];i++)
        {
            if (ld[i]==0)
            {
                g2d.drawImage(brtxt[ltt[i]],null,lx[level][i],ly[level][i]);
            }
            else if ((ld[i]>1 && ld[i]<5) || (ld[i]>11 && ld[i]<15)) g2d.drawImage(brtxt[8],null,lx[level][i],ly[level][i]);
            else if ((ld[i]>4 && ld[i]<8) || (ld[i]>14 && ld[i]<18)) g2d.drawImage(brtxt[9],null,lx[level][i],ly[level][i]);
            else if ((ld[i]>7 && ld[i]<11) || (ld[i]>17 && ld[i]<21)) g2d.drawImage(brtxt[10],null,lx[level][i],ly[level][i]);
        }
        if (bbonus==true)
        {
            r=new Rectangle2D.Float(xbonus,ybonus,25,25);
            g2d.drawImage(btxt[ibonus],null,(int)xbonus,(int)ybonus);
        }
        g2d.setColor(Color.white);
        g2d.setFont(new Font("TimesRoman",0,12));
        g2d.drawString("Player: "+points1,50,230);
        if (secplayer==false) g2d.drawString("Computer: "+points2,250,230);
        else g2d.drawString("Second Player: "+points2,250,230);
        if (remaining==-1)
        {
            g2d.setColor(Color.white);
            g2d.setFont(new Font("TimesRoman",0,15));
            g2d.drawString("You have cleared last level.",100,100);
        }
        if (bpaused==true)
        {
            g2d.setFont(new Font("Arial",0,20));
            g2d.setColor(Color.WHITE);
            g2d.drawString("PAUSED",140,180);
        }
    }

    public void mouseMoved(MouseEvent e)
    {
        if (bpaused==false && e.getX()-5-b1.width/2>0 && e.getX()-5+b1.width/2<390)
        {
            if (a.breleased==1) a.x += e.getX()-b1.width/2-5-b1.getX();
            b1.setX(e.getX()-(int)b1.width/2-5);
        }
        repaint();
    }
    public void mouseDragged(MouseEvent e){}
    public void mouseClicked(MouseEvent e)//LMB
    {
        if (e.getButton()==1 && a.breleased==1 && bpaused==false)
        {
            a.breleased=0;
        }
    }
    public void mouseExited(MouseEvent e){}
    public void mouseEntered(MouseEvent e){}
    public void mouseReleased(MouseEvent e){}
    public void mousePressed(MouseEvent e){}

    public void keyPressed(KeyEvent f)
    {
        int key = f.getKeyCode();
        if (key==81)//Q
        {
            level=0;
            points1=0;
            points2=0;
            a.playerbounced=1;
            setlevel();
        }
        else if (key==72)//H
        {
            if (secplayer==true && a.breleased==2) a.breleased=0;
            secplayer=!secplayer;
        }
        else if (key==80)//P
        {
            bpaused=!bpaused;
        }
        else if (secplayer==true && bpaused==false)
        {
            if (key==37)//left arrow
            {
                if (b2.x>20)
                {
                    if(a.breleased==2) a.x-=20;
                    b2.x-=20;
                }
                else
                {
                    b2.x=0;
                }
                repaint();
            }
            else if (key==39)//right arrow
            {
                if (b2.x+b2.width+20<390)
                {
                    if(a.breleased==2) a.x+=20;
                    b2.x+=20;
                }
                else
                {
                    b2.x=390-b2.width;
                }
                repaint();
            }
            else if(key==65) //a
            {
                if(b2.x>10)
                {
                    if(a.breleased==2) a.x-=10;
                    b2.x-=10;
                }
                else
                {
                    b2.x=0;
                }
                repaint();
            }
            else if (key==68)//d
            {
                if (b2.x+b2.width+10<390)
                {
                    if(a.breleased==2) a.x+=10;
                    b2.x+=10;
                }
                else
                {
                    b2.x=390-b2.width;
                }
                repaint();
            }
            else if (key==32)//Space
            {
                if (a.breleased==2) a.breleased=0;
            }
        }
    }

    public void keyReleased(KeyEvent f) {}
    public void keyTyped(KeyEvent f) {}
}

public class Program
{
    public static void main(String[] args)
    {
        javax.swing.SwingUtilities.invokeLater(new Runnable()
        {
            public void run()
            {
                int[][] lx={{0,1,2,3,4, 5,6,7,8,9, 10,11,12,0,1,2,3,4, 5,6,7,8,9, 10,11,12,0,1,2,3,4, 5,6,7,8,9, 10,11,12},
                        {0,0,6,6,12,12, 3,3,4,4,5,5,7,7,8,8,9,9, 0,0,1,1,6,6,11,11,12,12},
                        {0,1,6,11,12,0,1,6,11,12, 0,1,6,11,12,0,1,6,11,12, 4,5,6,7,8, 4,5,6,7,8, 4,5,6,7,8}};
                int[][] ly={{0,0,0,0,0, 0,0,0,0,0, 0,0,0, 1,1,1,1,1,1,1,1,1,1,1,1,1, -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
                        {-1,0,-1,0,-1,0, -2,1,-2,1,-2,1,-2,1,-2,1,-2,1, -3,2,-3,2,-3,2,-3,2,-3,2},
                        {-5,-5,-5,-5,-5,-4,-4,-4,-4,-4, 5,5,5,5,5,4,4,4,4,4, -1,-1,-1,-1,-1,0,0,0,0,0,1,1,1,1,1}};
                for (int i=0;i<3;i++)
                {
                    for(int j=0; j<lx[i].length;j++)
                    {
                        lx[i][j]= lx[i][j]*30;
                        ly[i][j]= ly[i][j]*10+180;
                    }
                }
                int[][] lt={{4,5,6,7,4, 5,6,7,4,5,6,7,4, 5,6,7,4,5, 6,7,4,5,6,7,4,5, 7,4,5,6,7, 4,5,6,7,4,5,6,7},
                        {0,0,0,0,0,0, 7,6,4,7,5,6,5,4,7,5,4,6, 1,1,1,1,1,1,1,1,1,1},
                        {2,2,2,2,2,1,1,1,1,1, 2,2,2,2,2,1,1,1,1,1, 6,5,5,5,6,4,7,0,7,4,6,5,5,5,6}};
                int[] lb={39,28,35};
                Board p;
                p=new Board(lx,ly,lb,lt);
                JFrame jf=new JFrame();
                jf.add(p);
                jf.setTitle("Arcanoid");
                jf.setSize(390,405);
                jf.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
                jf.setVisible(true);
            }
        });
    }
}
