//c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//allegro
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>

int LogicPerder;
int ScrW = 512, ScrH = 512;

ALLEGRO_DISPLAY * scr;

ALLEGRO_FONT * m_font, * i_font, * t_font, *counter_f;

ALLEGRO_BITMAP * bg_glow;
ALLEGRO_BITMAP * bframe;
ALLEGRO_BITMAP * sq;
ALLEGRO_BITMAP * sqb;
ALLEGRO_BITMAP * sqf;
ALLEGRO_BITMAP * zan;
ALLEGRO_BITMAP * bomba;

ALLEGRO_EVENT_QUEUE * eq;

ALLEGRO_TIMER * timer;

static ALLEGRO_COLOR black;
static ALLEGRO_COLOR white;


static ALLEGRO_COLOR corNum[8];


//float animCoef;

ALLEGRO_SAMPLE * bhover;
ALLEGRO_SAMPLE * bclick;
ALLEGRO_SAMPLE * back;
ALLEGRO_SAMPLE * start;
ALLEGRO_SAMPLE * quit;
ALLEGRO_SAMPLE * fudeu;
ALLEGRO_SAMPLE * boom;

ALLEGRO_SAMPLE_ID * InidiceSom;

//Funções Globais

int i, j, k;
unsigned short int jogadas = 0;
int IniciarJogo(char*);

bool redraw;
bool Mouse_em_Cima = false;
bool clicavel = true;

int selX = 0, selY = 1024;
int MinaAtual[4] = {0, 0, 0, 0};

float ra, ga, ba, bga;
double cronometro;
double tempoDeJogo;

double tempoInicio;

void DesenharMenuInicial();
void IniciarPosMouse(ALLEGRO_EVENT);
bool InicLogicBotaoMouse(ALLEGRO_EVENT);

void DesenharMenuDif();
void LogPosMouseMenuDif(ALLEGRO_EVENT);
bool LogicBotaoMenuDif(ALLEGRO_EVENT);

void DesenharComoJogar();
void LogPosMouseComoJogar(ALLEGRO_EVENT);
bool LogicBotaoComoJogar(ALLEGRO_EVENT);

bool CriarCampoMinado(int, int, int, bool);

void DesenharJogo();
void LogicPosMouseJogo(ALLEGRO_EVENT);
bool LogicBotaoJogo(ALLEGRO_EVENT);
unsigned char ClicarMina(unsigned char, unsigned char, unsigned char);

void FecharTudo();

int err(unsigned char);

//ponteiros de funcao

void(*curDFunc)();
void(*curMAxLFunc)(ALLEGRO_EVENT);
bool(*curMBuLFunc)(ALLEGRO_EVENT);


// lógica do jogo em si

typedef struct c_minado {

    short unsigned int m;
    short unsigned n;
    int mnum;
    int fnum;
    bool zanetti;

    bool minas[64][64];
    signed char CampoJogador[64][64];

} c_minado;

c_minado CampoMinado;

int IniciarJogo(char title[]) {
    if (!al_init())
        return err(0);

    if (!al_init_image_addon())
        return err(1);

    if (!al_install_keyboard())
        return err(2);

    if (!al_install_mouse())
        return err(3);

    timer = al_create_timer(1.0 / 100.0);
    if (!timer)
        return err(4);

    eq = al_create_event_queue();

    if (!eq)
        return err(5);

    if (!al_init_primitives_addon())
        return err(6);

    if (!al_install_audio())
        return err(7);

    if (!al_init_acodec_addon())
        return err(8);

    if (!al_reserve_samples(8))
        return err(9);

    bhover = al_load_sample("sfx/buttonhover.wav");
    bclick = al_load_sample("sfx/buttonclick.wav");
    back = al_load_sample("sfx/back.wav");
    start = al_load_sample("sfx/start.wav");
    quit = al_load_sample("sfx/quit.wav");
    fudeu = al_load_sample("sfx/fudeu.ogg");
    boom = al_load_sample("sfx/bomba.ogg");

    InidiceSom = al_create_sample_instance(bhover);

    al_register_event_source(eq, al_get_timer_event_source(timer));

    al_register_event_source(eq, al_get_mouse_event_source());

    al_init_font_addon();

    al_init_ttf_addon();

    t_font = al_load_ttf_font("res/square-deal.ttf", 34, 0);
    counter_f = al_load_ttf_font("res/square-deal.ttf", 24, 0);
    m_font = al_load_ttf_font("res/alterebro-pixel-font.ttf", 36, 0);
    i_font = al_load_ttf_font("res/alterebro-pixel-font.ttf", 30, 0);

    scr = al_create_display(ScrW, ScrH);

    al_set_window_title(scr, title);

    bg_glow = al_load_bitmap("gfx/bg_glow.png");
    bframe = al_load_bitmap("gfx/glass.png");
    sq = al_load_bitmap("gfx/sq.png");
    sqb = al_load_bitmap("gfx/sqb.png");
    sqf = al_load_bitmap("gfx/sqf.png");
    zan = al_load_bitmap("gfx/zan.png");
    bomba = al_load_bitmap("gfx/bomba.png");


    al_flip_display();
    al_start_timer(timer);

    white = al_map_rgb(255, 255, 255);
    black = al_map_rgb(0, 0, 0);

    corNum[0] = al_map_rgb(2, 0, 255);
    corNum[1] = al_map_rgb(2, 128, 1);
    corNum[2] = al_map_rgb(253, 0, 1);
    corNum[3] = al_map_rgb(4, 0, 137);
    corNum[4] = al_map_rgb(131, 0, 3);
    corNum[5] = al_map_rgb(0, 128, 128);
    corNum[6] = al_map_rgb(0, 0, 0);
    corNum[7] = al_map_rgb(128, 128, 128);

    return 1;
}

int main() {

    if (!IniciarJogo("Campo Minado")) // se não conseguir inicializar o jogo, cancela tudo
        return -1;

    srand(time(NULL));
//pra reduzir o numero de comparações no loop do jogo
    //ALLEGRO_EVENT ev;
    curDFunc = &DesenharMenuInicial; //começa com as funcoes do menu
    curMAxLFunc = &IniciarPosMouse;
    curMBuLFunc = &InicLogicBotaoMouse;

    bool quit = true; // pra n ter que colocar um ! no while

    while (quit) {

        ALLEGRO_EVENT ev;
        al_wait_for_event(eq, & ev);

        if (ev.type == ALLEGRO_EVENT_TIMER) {
            redraw = true;

        } else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            quit = false;

        } else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {

            curMAxLFunc(ev);

        } else if (ev.type == ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY) {

            Mouse_em_Cima = false;
            curMAxLFunc(ev);

        } else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {

            quit = curMBuLFunc(ev);

        }

        if (redraw && al_is_event_queue_empty(eq)) {
            redraw = false;
            curDFunc();
        }
    }

    FecharTudo();

    return 0;
}

void DesenharMenuInicial() {

    cronometro = al_get_time();
    al_clear_to_color(al_map_rgb(0, 0, 0));

    bga = (cos((2 * cronometro) + 2)/4)+0.75;

    al_color_hsl_to_rgb((sin(cronometro/4)*180)+180, 1.0, 0.3, &ra, &ga, &ba);

    al_draw_tinted_bitmap(bg_glow, al_map_rgba_f(ra * bga, ga * bga, ba * bga, bga), 0, 0, NULL);

    al_draw_text(t_font, white, ScrW / 2, 128 - sin(cronometro * 2) * 9, ALLEGRO_ALIGN_CENTRE, "CAMPO MINADO");

    al_draw_text(m_font, white, ScrW / 2, 238, ALLEGRO_ALIGN_CENTER, "INICIAR");
    al_draw_text(m_font, white, ScrW / 2, 274, ALLEGRO_ALIGN_CENTER, "COMO JOGAR");

    al_draw_text(m_font, white, ScrW / 2, 398, ALLEGRO_ALIGN_CENTER, "SAIR");

    al_draw_text(m_font, al_map_rgb(255 - (255*ra), 255 - (255*ga), 255 - (255*ba)), (ScrW / 2) - sin(cronometro) * 280, 458, ALLEGRO_ALIGN_CENTER, "DESENVOLVIDO POR ANDREY AVILA, GABRIEL TANSKI E JUAREZ CORNELI FILHO - TELECOM 2018/1 UFSM");

    al_draw_text(m_font, white, ScrW / 2, selY, ALLEGRO_ALIGN_CENTER, "#                            #");
    al_draw_rectangle(ScrW / 2 - 128, selY, ScrW / 2 + 128, selY + 36, white, 2);

    al_flip_display();

}

void IniciarPosMouse(ALLEGRO_EVENT ev) {
    if (ev.mouse.y <= 272 && ev.mouse.y >= 240) {
        if (!Mouse_em_Cima) {
            al_stop_sample(InidiceSom);
            Mouse_em_Cima = true;
            al_play_sample(bhover, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        }
        selY = 238;
    } else if (ev.mouse.y <= 308 && ev.mouse.y >= 276) {
        if (!Mouse_em_Cima) {
            al_stop_sample(InidiceSom);
            Mouse_em_Cima = true;
            al_play_sample(bhover, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        }
        selY = 274;

    } else if (ev.mouse.y <= 434 && ev.mouse.y >= 400) {
        if (!Mouse_em_Cima) {
            al_stop_sample(InidiceSom);
            Mouse_em_Cima = true;
            al_play_sample(bhover, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        }
        selY = 398;
    } else {
        Mouse_em_Cima = false;
        selY = 1024;
    }
}

bool InicLogicBotaoMouse(ALLEGRO_EVENT ev) {

    if(!Mouse_em_Cima)
        return true;
    if (ev.mouse.y <= 272 && ev.mouse.y >= 240) {
        al_play_sample(bclick, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        if(clicavel)
        {
            curDFunc = &DesenharMenuDif;
            curMAxLFunc = &LogPosMouseMenuDif;
            curMBuLFunc = &LogicBotaoMenuDif;
            selY = 1024;
            Mouse_em_Cima = false;
        }
    } else if (ev.mouse.y <= 308 && ev.mouse.y >= 276) {
        al_play_sample(bclick, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        if(clicavel)
        {
            curDFunc = &DesenharComoJogar;
            curMAxLFunc = &LogPosMouseComoJogar;
            curMBuLFunc = &LogicBotaoComoJogar;
            selY = 1024;
            Mouse_em_Cima = false;
        }
    } else if (ev.mouse.y <= 344 && ev.mouse.y >= 316) {
        al_play_sample(bclick, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
    } else if (ev.mouse.y <= 434 && ev.mouse.y >= 400) {
        al_play_sample(back, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        return false;
    } else {
        Mouse_em_Cima = false;
        selY = 1024;
        return true;
    }
}

void DesenharMenuDif() {

    cronometro = al_get_time();
    al_clear_to_color(al_map_rgb(0, 0, 0));

    bga = (cos((2 * cronometro) + 2)/4)+0.75;

    al_color_hsl_to_rgb((sin((cronometro+3)/4)*180)+180, 1, 0.3, &ra, &ga, &ba);

    al_draw_tinted_bitmap(bg_glow, al_map_rgba_f(ra, ga, ba, bga), 0, 0, NULL);

    al_draw_text(t_font, white, ScrW / 2, 60 - sin(cronometro * 2) * 9, ALLEGRO_ALIGN_CENTRE, "Escolha a Dificuldade");

    al_draw_text(m_font, white, ScrW / 2, 178, ALLEGRO_ALIGN_CENTER, "INICIANTE");
    al_draw_text(m_font, white, ScrW / 2, 214, ALLEGRO_ALIGN_CENTER, "INTERMEDIARIO");
    al_draw_text(m_font, white, ScrW / 2, 250, ALLEGRO_ALIGN_CENTER, "AVANCADO");
    al_draw_text(m_font, white, ScrW / 2, 286, ALLEGRO_ALIGN_CENTER, "ZANETTI");

    al_draw_text(m_font, white, ScrW / 2, 458, ALLEGRO_ALIGN_CENTER, "VOLTAR PARA MENU INICIAL");

    al_draw_text(m_font, white, ScrW / 2, selY, ALLEGRO_ALIGN_CENTER, "#                                      #");
    al_draw_rectangle(ScrW / 2 - 128, selY, ScrW / 2 + 128, selY + 36, white, 2);

    al_flip_display();

}

void LogPosMouseMenuDif(ALLEGRO_EVENT ev) {

    if (ev.mouse.y <= 212 && ev.mouse.y >= 180) {
        if (!Mouse_em_Cima) {
            al_stop_sample(InidiceSom);
            Mouse_em_Cima = true;
            al_play_sample(bhover, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        }
        selY = 178;
    } else if (ev.mouse.y <= 248 && ev.mouse.y >= 216) {
        if (!Mouse_em_Cima) {
            al_stop_sample(InidiceSom);
            Mouse_em_Cima = true;
            al_play_sample(bhover, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        }
        selY = 214;
    } else if (ev.mouse.y <= 284 && ev.mouse.y >= 256) {
        if (!Mouse_em_Cima) {
            Mouse_em_Cima = true;
            al_stop_sample(InidiceSom);
            al_play_sample(bhover, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        }
        selY = 250;
    } else if (ev.mouse.y <= 320 && ev.mouse.y >= 288) {
        if (!Mouse_em_Cima) {
            al_stop_sample(InidiceSom);
            Mouse_em_Cima = true;
            al_play_sample(bhover, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        }
        selY = 286;
    } else if (ev.mouse.y <= 494 && ev.mouse.y >= 460) {
        if (!Mouse_em_Cima) {
            al_stop_sample(InidiceSom);
            Mouse_em_Cima = true;
            al_play_sample(bhover, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        }
        selY = 458;
    } else {
        Mouse_em_Cima = false;
        selY = 1024;
    }
}

bool LogicBotaoMenuDif(ALLEGRO_EVENT ev) {

    if(!Mouse_em_Cima)
        return true;
    if (ev.mouse.y <= 212 && ev.mouse.y >= 180) {
        al_play_sample(start, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);

        //if(!CriarCampoMinado(9, 9, 10))

        CriarCampoMinado(9, 9, 10, false);
        //return err(0);

        curDFunc = &DesenharJogo;
        curMAxLFunc = &LogicPosMouseJogo;
        curMBuLFunc = &LogicBotaoJogo;
        selY = 1024;
        Mouse_em_Cima = false;
    } else if (ev.mouse.y <= 248 && ev.mouse.y >= 216) {
        al_play_sample(start, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        CriarCampoMinado(16, 16, 40, false);
        //return err(0);

        curDFunc = &DesenharJogo;
        curMAxLFunc = &LogicPosMouseJogo;
        curMBuLFunc = &LogicBotaoJogo;
        selY = 1024;
        Mouse_em_Cima = false;
    } else if (ev.mouse.y <= 284 && ev.mouse.y >= 256) {
        al_play_sample(start, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        CriarCampoMinado(30, 16, 99, false);
        //return err(0);

        curDFunc = &DesenharJogo;
        curMAxLFunc = &LogicPosMouseJogo;
        curMBuLFunc = &LogicBotaoJogo;

        selY = 1024;
        Mouse_em_Cima = false;
    } else if (ev.mouse.y <= 320 && ev.mouse.y >= 288) {
        al_play_sample(start, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        CriarCampoMinado(16, 16, 254, true);
        al_play_sample(fudeu, 1, 0, 1, ALLEGRO_PLAYMODE_LOOP, InidiceSom + 1);
        //return err(0);

        curDFunc = &DesenharJogo;
        curMAxLFunc = &LogicPosMouseJogo;
        curMBuLFunc = &LogicBotaoJogo;
        selY = 1024;
        Mouse_em_Cima = false;
    } else if (ev.mouse.y <= 494 && ev.mouse.y >= 460) {
        al_play_sample(back, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);

        curDFunc = &DesenharMenuInicial;
        curMAxLFunc = &IniciarPosMouse;
        curMBuLFunc = &InicLogicBotaoMouse;
        selY = 1024;
        Mouse_em_Cima = false;

    } else {
        Mouse_em_Cima = false;
        selY = 1024;
        return true;
    }
}

void DesenharComoJogar(){

    cronometro = al_get_time();
    al_clear_to_color(al_map_rgb(0, 0, 0));

    bga = (cos((2 * cronometro) + 2)/4)+0.75;

    al_color_hsl_to_rgb((sin(cronometro/4)*180)+180, 1.0, 0.3, &ra, &ga, &ba);

    al_draw_tinted_bitmap(bg_glow, al_map_rgba_f(ra * bga, ga * bga, ba * bga, bga), 0, 0, NULL);

    al_draw_text(t_font, white, ScrW / 2, 50 - sin(cronometro * 2) * 9, ALLEGRO_ALIGN_CENTRE, "REGRAS E DICAS");

    al_draw_text(m_font, white, ScrW / 2, 130, ALLEGRO_ALIGN_CENTER, " # AO CLICAR EM UMA MINA, O JOGO ACABA.");
    al_draw_text(m_font, white, ScrW / 2, 190, ALLEGRO_ALIGN_CENTER, " # SE DESCOBRIR UM QUADRADO VAZIO, O JOGO CONTINUA.");
    al_draw_text(m_font, white, ScrW / 2, 250, ALLEGRO_ALIGN_CENTER, " # O NUMERO INFORMA A QUANTIA DE MINAS");
    al_draw_text(m_font, white, ScrW / 2, 270, ALLEGRO_ALIGN_CENTER, "ESCONDIDAS NOS 8 QUADRADOS MAIS PROXIMOS.");
    al_draw_text(m_font, white, ScrW / 2, 320, ALLEGRO_ALIGN_CENTER, " # AO SUSPEITAR DE UMA MINA, CLIQUE COM BOTAO");
    al_draw_text(m_font, white, ScrW / 2, 340, ALLEGRO_ALIGN_CENTER, "DIREITO, ISSO MARCARA O LOCAL COM UMA BANDEIRA.");

    al_draw_text(m_font, white, ScrW / 2, 458, ALLEGRO_ALIGN_CENTER, "VOLTAR AO MENU INICIAL");

    al_draw_text(m_font, white, ScrW / 2, selY, ALLEGRO_ALIGN_CENTER, "#                                      #");
    al_draw_rectangle(ScrW / 2 - 128, selY, ScrW / 2 + 128, selY + 36, white, 2);

    al_flip_display();


}

void LogPosMouseComoJogar(ALLEGRO_EVENT ev){


    if (ev.mouse.y <= 494 && ev.mouse.y >= 460) {
        if (!Mouse_em_Cima) {
            al_stop_sample(InidiceSom);
            Mouse_em_Cima = true;
            al_play_sample(bhover, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        }
        selY = 458;
    } else {
        Mouse_em_Cima = false;
        selY = 1024;
    }
}

bool LogicBotaoComoJogar(ALLEGRO_EVENT ev){


    if (ev.mouse.y <= 494 && ev.mouse.y >= 460) {
        al_play_sample(back, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        al_stop_sample(InidiceSom + 1);

        curDFunc = &DesenharMenuInicial;
        curMAxLFunc = &IniciarPosMouse;
        curMBuLFunc = &InicLogicBotaoMouse;
        selY = 1024;
        Mouse_em_Cima = false;
    } else {
        Mouse_em_Cima = false;
        selY = 1024;
    }

    return true;
}

bool CriarCampoMinado(int m, int n, int num, bool god) {

    //printf("tInicio %lf\n", tempoInicio);
    tempoInicio = al_get_time();
    jogadas = 0;
    //printf("tNovo Inicio %lf\n", tempoInicio);
    CampoMinado.m = m;
    CampoMinado.n = n;
    CampoMinado.mnum = num;
    CampoMinado.fnum = 0;
    CampoMinado.zanetti = god;
    LogicPerder = 0;

    for(i = 0; i<m; i++)
        for(j=0; j<n; j++)
            CampoMinado.minas[i][j] = false;

    for(i = 0; i<m; i++)
        for(j=0; j<n; j++)
            CampoMinado.CampoJogador[i][j] = -1;

    /*for(i = 0; i<(num); i++)
    {
        rX = rand()%m;
        rY = rand()%n;

        if(CampoMinado.minas[rX][rY] == false)
            CampoMinado.minas[rX][rY] = true;
        else
            i--;
    }*/

    return true;
}

void DesenharJogo() {

    cronometro = al_get_time();
    al_clear_to_color(al_map_rgb(0, 0, 0));

    bga = (cos((2 * cronometro) + 2)/4)+0.75;

    al_color_hsl_to_rgb((sin((((1 + (100 * CampoMinado.zanetti)) * (cronometro)-4))/8)*180)+180, 1, 0.3, &ra, &ga, &ba);

    al_draw_tinted_bitmap(bg_glow, al_map_rgba_f(ra, ga, ba, bga), 0, 0, NULL);


    if (!LogicPerder)
    {
        tempoDeJogo = al_get_time() - tempoInicio;
    }

    al_draw_textf(t_font, white, ScrW / 2, 75, ALLEGRO_ALIGN_CENTER, "%02im%02is", (int) (tempoDeJogo/60), (int) (tempoDeJogo)%60);

    al_draw_textf(counter_f, white, ScrW / 5, 40, ALLEGRO_ALIGN_CENTER, "%u Jogadas", jogadas);
    al_draw_textf(counter_f, white, ScrW * 0.8, 40, ALLEGRO_ALIGN_CENTER, "%02i Minas", (int) (CampoMinado.mnum - CampoMinado.fnum));

    for(i = 0; i < CampoMinado.m; i++)
    {
        for(j = 0; j < CampoMinado.n; j++) {
                if(CampoMinado.CampoJogador[i][j] <0)
                    al_draw_bitmap(sq, ScrW/2 - (CampoMinado.m * 8) + (i*16), ScrH/2 - (CampoMinado.n * 8) + (j*16), NULL);
                else
                    al_draw_bitmap(sqb, ScrW/2 - (CampoMinado.m * 8) + (i*16), ScrH/2 - (CampoMinado.n * 8) + (j*16), NULL);
            if(CampoMinado.CampoJogador[i][j] !=-1)
            {
                if(CampoMinado.CampoJogador[i][j] == -2)
                {
                    al_draw_bitmap(sqf, ScrW/2 - (CampoMinado.m * 8) + (i*16) , ScrH/2 - (CampoMinado.n * 8) + (j*16)  , NULL);

                } else {
                    al_draw_bitmap(sqb, ScrW/2 - (CampoMinado.m * 8) + (i*16) , ScrH/2 - (CampoMinado.n * 8) + (j*16) +2 , NULL);
                    if(CampoMinado.CampoJogador[i][j]>0)
                        al_draw_textf(i_font, corNum[CampoMinado.CampoJogador[i][j]-1], ScrW/2 - (CampoMinado.m * 8) + (i*16) + 8, ScrH/2 - (CampoMinado.n * 8) + (j*16) - 8, ALLEGRO_ALIGN_CENTER, "%i", CampoMinado.CampoJogador[i][j]);
                }
            }

            if(CampoMinado.minas[i][j])
            {
                if( LogicPerder == 1)
                    {
                        if(CampoMinado.zanetti){
                            al_draw_bitmap(bomba, ScrW/2 - (CampoMinado.m * 8) + (i*16) , ScrH/2 - (CampoMinado.n * 8) + (j*16) + 2, NULL);
                            al_draw_text(m_font, white, ScrW / 2, 400, ALLEGRO_ALIGN_CENTER, "SUA MATRICULA FOI CANCELADA");
                        }
                        else{
                            al_draw_bitmap(bomba, ScrW/2 - (CampoMinado.m * 8) + (i*16) , ScrH/2 - (CampoMinado.n * 8) + (j*16) + 2, NULL);
                            al_draw_text(m_font, white, ScrW / 2, 400, ALLEGRO_ALIGN_CENTER, "INFELIZMENTE VOCE PERDEU!");
                        }
                    }
                else if( LogicPerder == 2)
                    {
                        if(CampoMinado.zanetti){
                            al_draw_bitmap(bomba, ScrW/2 - (CampoMinado.m * 8) + (i*16) , ScrH/2 - (CampoMinado.n * 8) + (j*16) , NULL);
                            al_draw_text(m_font, white, ScrW / 2, 400, ALLEGRO_ALIGN_CENTER, "VOCÊ FOI APROVADO NA DISCIPLINA");
                        }
                        else{
                            al_draw_bitmap(bomba, ScrW/2 - (CampoMinado.m * 8) + (i*16) , ScrH/2 - (CampoMinado.n * 8) + (j*16) , NULL);
                            al_draw_text(m_font, white, ScrW / 2, 400, ALLEGRO_ALIGN_CENTER, "VOCÊ VENCEU!");
                        }
                    }

            }
        }
    }


    al_draw_text(m_font, white, ScrW / 2, 458, ALLEGRO_ALIGN_CENTER, "VOLTAR PARA MENU INICIAL");

    al_draw_text(m_font, white, ScrW / 2, selY, ALLEGRO_ALIGN_CENTER, "#                                      #");
    al_draw_rectangle(ScrW / 2 - 128, selY, ScrW / 2 + 128, selY + 36, white, 2);

    al_flip_display();

}

void LogicPosMouseJogo(ALLEGRO_EVENT ev) {

    MinaAtual[0] = (ev.mouse.x - (ScrW/2) + (CampoMinado.m*8));
    MinaAtual[1] = (ev.mouse.y - (ScrH/2) + (CampoMinado.n*8));
    MinaAtual[2] = (ev.mouse.x - (ScrW/2) + (CampoMinado.m*8))/16;
    MinaAtual[3] = (ev.mouse.y - (ScrH/2) + (CampoMinado.n*8))/16;

    //printf("Quad: %i x %i eq %i %i\n", MinaAtual[0], MinaAtual[1], MinaAtual[2], MinaAtual[3]);

    if (ev.mouse.y <= 494 && ev.mouse.y >= 460) {
        if (!Mouse_em_Cima) {
            al_stop_sample(InidiceSom);
            Mouse_em_Cima = true;
            al_play_sample(bhover, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        }
        selY = 458;
    } else {
        Mouse_em_Cima = false;
        selY = 1024;
    }

}

bool LogicBotaoJogo(ALLEGRO_EVENT ev) {

    if(ev.mouse.button == 1)
        ClicarMina(MinaAtual[2], MinaAtual[3], 1);
    else
         ClicarMina(MinaAtual[2], MinaAtual[3], 2);

    if(!Mouse_em_Cima)
        return true;

    if (ev.mouse.y <= 272 && ev.mouse.y >= 240) {
        al_play_sample(bclick, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        if(clicavel)
        {
            curDFunc = &DesenharMenuDif;
            curMAxLFunc = &LogPosMouseMenuDif;
            curMBuLFunc = &LogicBotaoMenuDif;
            selY = 1024;
            Mouse_em_Cima = false;
        }
    } else if (ev.mouse.y <= 308 && ev.mouse.y >= 276) {
        al_play_sample(bclick, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
    } else if (ev.mouse.y <= 344 && ev.mouse.y >= 316) {
        al_play_sample(bclick, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
    } else if (ev.mouse.y <= 434 && ev.mouse.y >= 400) {
        al_play_sample(back, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        return false;
    } else if (ev.mouse.y <= 494 && ev.mouse.y >= 460) {
        al_play_sample(back, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        al_stop_sample(InidiceSom + 1);

        curDFunc = &DesenharMenuInicial;
        curMAxLFunc = &IniciarPosMouse;
        curMBuLFunc = &InicLogicBotaoMouse;
        selY = 1024;
        Mouse_em_Cima = false;

    } else {
        Mouse_em_Cima = false;
        selY = 1024;
        return true;
    }
}

unsigned char ClicarMina(unsigned char x, unsigned char y, unsigned char comando) // x y = posicao do quadrado; comando = comando: 1 é abrir normal, 2 é colocar bandeira, 3 é quando o zero abre em volta
{

    if(LogicPerder>0)
        return 0;

    if ((comando == 1) && (CampoMinado.CampoJogador[x][y] >= 0))
            return 0;

    //printf("\n\nJOGADAS %i", jogadas);

    if(x <= CampoMinado.m && x>=0 && y <= CampoMinado.n && y>=0)
    {

        if(comando == 1)
        {

            if(CampoMinado.CampoJogador[x][y] >= 0)
                return 0;

            if(jogadas == 0){
                //printf("NUM MINAS %i\n", CampoMinado.mnum);
                unsigned short int rX, rY;

                for(i = 0; i<(CampoMinado.mnum); i++)
                {
                    //printf("mina %i\n", i);
                    rX = rand()%CampoMinado.m;
                    rY = rand()%CampoMinado.n;

                    if((CampoMinado.minas[rX][rY] == false) && ((rX != x) || (rY != y)))
                        CampoMinado.minas[rX][rY] = true;
                    else
                        if(i>-20){
                            i--;
                        } else {
                            break;
                        }
                }

                //printf("IIIII %i\n", i);
            }

            jogadas++;
        }

        if(comando == 2)
        {
            if(CampoMinado.CampoJogador[x][y] == -2){
                CampoMinado.CampoJogador[x][y] = -1;
                CampoMinado.fnum -= CampoMinado.fnum <= CampoMinado.mnum;
            }
            else if (CampoMinado.fnum <= CampoMinado.mnum){

                CampoMinado.CampoJogador[x][y] = -2;
                CampoMinado.fnum += CampoMinado.fnum < CampoMinado.mnum;
            }
            return 0;

        }

        if(CampoMinado.CampoJogador[x][y] >= 0)
            return 0;

        unsigned char contador = 0;

        if(CampoMinado.minas[x][y])
        {
            for(i = 0; i<CampoMinado.m; i++)
                for(j=0; j<CampoMinado.n; j++)
                    if(CampoMinado.CampoJogador[i][j] == -2 && (CampoMinado.minas[i][j]))
                        CampoMinado.CampoJogador[i][j] = 0;

            LogicPerder = 1;

            return 0;

        }

        for(i = (x-1); i<=(x+1); i++)
        {
            for(j = y-1; j<=(y+1); j++)
            {
                if((i >= 0) && (j>=0) && (i < CampoMinado.m) && (j < CampoMinado.n) && CampoMinado.minas[i][j])
                    contador++;
            }
        }

        CampoMinado.CampoJogador[x][y] = contador;

        if(contador==0)
        {

            if(CampoMinado.CampoJogador[x-1][y-1] > -2 && !CampoMinado.minas[x-1][y-1])
                    {
                        //printf("CL\n");
                        ClicarMina(x-1, y-1, 3);
                    }

            if(CampoMinado.CampoJogador[x][y-1] > -2 && !CampoMinado.minas[x][y-1])
                    {
                        //printf("CL\n");
                        ClicarMina(x, y-1, 3);
                    }

            if(CampoMinado.CampoJogador[x+1][y-1] > -2 && !CampoMinado.minas[x+1][y-1])
                    {
                        //printf("CL\n");
                        ClicarMina(x+1, y-1, 3);
                    }
            if(CampoMinado.CampoJogador[x-1][y] > -2 && !CampoMinado.minas[x-1][y])
                    {
                        //printf("CL\n");
                        ClicarMina(x-1, y, 3);
                    }
            if(CampoMinado.CampoJogador[x+1][y] > -2 && !CampoMinado.minas[x+1][y])
                    {
                        //printf("CL\n");
                        ClicarMina(x+1, y, 3);
                    }
            if(CampoMinado.CampoJogador[x-1][y+1] > -2 && !CampoMinado.minas[x-1][y+1])
                    {
                        //printf("CL\n");
                        ClicarMina(x-1, y+1, 3);
                    }
            if(CampoMinado.CampoJogador[x][y+1] > -2 && !CampoMinado.minas[x][y+1])
                    {
                        //printf("CL\n");
                        ClicarMina(x, y+1, 3);
                    }
            if(CampoMinado.CampoJogador[x+1][y+1] > -2 && !CampoMinado.minas[x+1][y+1])
                    {
                        //printf("CL\n");
                        ClicarMina(x+1, y+1, 3);
                    }
        }

        //printf("X Y %i %i\n", i, j);

        contador = 0;
        if(comando == 1 || comando == 3)
        {
            for(i = 0; i<CampoMinado.m; i++)
                for(j=0; j<CampoMinado.n; j++)
                    if((CampoMinado.CampoJogador[i][j] == -2 || CampoMinado.CampoJogador[i][j] == -1))
                        contador++;

            if(contador == CampoMinado.mnum){
                LogicPerder = 2;
                return 0;
            }

        }

    }



}

void FecharTudo() {

    al_destroy_bitmap(bg_glow);
    al_destroy_bitmap(bframe);
    al_destroy_font(m_font);
    al_destroy_font(i_font);
    al_destroy_font(t_font);
    al_destroy_sample(bhover);
    al_destroy_sample(bclick);
    al_destroy_sample(back);
    al_destroy_sample(start);
    al_destroy_sample(quit);
    al_destroy_timer(timer);
    al_destroy_display(scr);
    al_destroy_event_queue(eq);
}

int err(unsigned char id) {

    switch (id) {
    case 0:
        al_show_native_message_box(
            scr,
            "Erro",
            "",
            "O programa não pôde inicializar o Allegro.",
            NULL,
            ALLEGRO_MESSAGEBOX_ERROR
        );
        break;

    case 1:
        al_show_native_message_box(
            scr,
            "Erro",
            "",
            "O programa não pôde inicializar o módulo de imagens.",
            NULL,
            ALLEGRO_MESSAGEBOX_ERROR
        );
        break;

    case 2:
        al_show_native_message_box(
            scr,
            "Erro",
            "",
            "O programa não pôde inicializar o módulo do teclado.",
            NULL,
            ALLEGRO_MESSAGEBOX_ERROR
        );
        break;

    case 3:
        al_show_native_message_box(
            scr,
            "Erro",
            "",
            "O programa não pôde inicializar o módulo do mouse.",
            NULL,
            ALLEGRO_MESSAGEBOX_ERROR
        );
        break;

    case 4:
        al_show_native_message_box(
            scr,
            "Erro",
            "",
            "O programa não pôde inicializar o módulo do cronômetro.",
            NULL,
            ALLEGRO_MESSAGEBOX_ERROR
        );
        break;

    case 5:
        al_show_native_message_box(
            scr,
            "Erro",
            "",
            "O programa não pôde inicializar o módulo da fila de eventos.",
            NULL,
            ALLEGRO_MESSAGEBOX_ERROR
        );
        break;

    case 6:
        al_show_native_message_box(
            scr,
            "Erro",
            "",
            "O programa não pôde inicializar o módulo de desenho de primitivas.",
            NULL,
            ALLEGRO_MESSAGEBOX_ERROR
        );
        break;

    case 7:
        al_show_native_message_box(
            scr,
            "Erro",
            "",
            "O programa não pôde inicializar o módulo de áudio.",
            NULL,
            ALLEGRO_MESSAGEBOX_ERROR
        );
        break;

    case 8:
        al_show_native_message_box(
            scr,
            "Erro",
            "",
            "O programa não pôde inicializar o módulo dos codecs.",
            NULL,
            ALLEGRO_MESSAGEBOX_ERROR
        );
        break;

    case 9:
        al_show_native_message_box(
            scr,
            "Erro",
            "",
            "O programa não pôde reservar amostras de áudio.",
            NULL,
            ALLEGRO_MESSAGEBOX_ERROR
        );
        break;

    default:
        al_show_native_message_box(
            scr,
            "Erro",
            "",
            "Erro indefinido.",
            NULL,
            ALLEGRO_MESSAGEBOX_ERROR
        );
        break;
    }

    return false;
}
