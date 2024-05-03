#include "displayapp/screens/Maze.h"
#include "displayapp/DisplayApp.h"
#include <lv_conf.h>

using namespace Pinetime::Applications::Screens;

int X(int n) {
    return n%N_CELLE;
}
int Y(int n) {
    return n/N_CELLE;
}
int XY(int x,int y) {
    return (y*N_CELLE)+x;
}

void generate_maze(Pinetime::Controllers::MotionController motionController, bool muri_oriz[N_CELLE*N_CELLE+N_CELLE], bool muri_vert[N_CELLE*N_CELLE+N_CELLE], int *destinazione) {
    int stack[N_CELLE*N_CELLE];
    int stack_top = 0;

    bool visited[N_CELLE*N_CELLE];
    for (int i=0; i<N_CELLE; i++) {
        for (int j=0; j<N_CELLE; j++) {
            visited[XY(i,j)] = false;
        }
    }

    visited[0] = true;
    stack[stack_top++] = 0;
    int rand_accumulator = 0b1111;
    int stack_max = 0;
    while (stack_top>0) {
        rand_accumulator++;
        int corrente = stack[--stack_top];
        if (stack_top>stack_max) {
            stack_max = stack_top;
            *destinazione = corrente;
        }
        int vicini[4];
        int n_vicini = 0;
        if (X(corrente)>0 && !visited[XY(X(corrente)-1,Y(corrente))]) {
            vicini[n_vicini++] = XY(X(corrente)-1,Y(corrente));
        }
        if (Y(corrente)>0 && !visited[XY(X(corrente),Y(corrente)-1)]) {
            vicini[n_vicini++] = XY(X(corrente),Y(corrente)-1);
        }
        if (X(corrente)<N_CELLE-1 && !visited[XY(X(corrente)+1,Y(corrente))]) {
            vicini[n_vicini++] = XY(X(corrente)+1,Y(corrente));
        }
        if (Y(corrente)<N_CELLE-1 && !visited[XY(X(corrente),Y(corrente)+1)]) {
            vicini[n_vicini++] = XY(X(corrente),Y(corrente)+1);
        }
        if (n_vicini>0) {
            stack[stack_top++] = corrente;
            int rand_new = (motionController.X()&0b1111);
            rand_new += (motionController.Y()&0b1111);
            rand_new += (motionController.Z()&0b1111);
            rand_accumulator = (rand_accumulator)^(rand_new<<1);
            int rand = rand_accumulator % n_vicini;

            int vicino = vicini[rand];
            if (X(vicino)<X(corrente)) {
                muri_vert[corrente] = false;
            }
            if (X(vicino)>X(corrente)) {
                muri_vert[corrente+1] = false;
            }
            if (Y(vicino)<Y(corrente)) {
                muri_oriz[corrente] = false;
            }
            if (Y(vicino)>Y(corrente)) {
                muri_oriz[corrente+N_CELLE] = false;
            }
            visited[vicino] = true;
            stack[stack_top++] = vicino;
        }
    }
}

void crea_muro(lv_style_t &stile, lv_point_t *punti) {
    lv_obj_t *muro = lv_line_create(lv_scr_act(), nullptr);
    lv_obj_add_style(muro, LV_LINE_PART_MAIN, &stile);
    lv_obj_align(muro, nullptr, LV_ALIGN_IN_TOP_LEFT, 0,0);
    lv_line_set_points(muro, punti, 2);
}

Maze::Maze(Controllers::MotionController& motionController) : motionController{motionController} {
    const int CELL_SIZE = 240/N_CELLE;
    won = false;

    lv_style_init(&stile_linea);
    lv_style_set_line_width(&stile_linea, LV_STATE_DEFAULT, 2);
    lv_style_set_line_color(&stile_linea, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_line_rounded(&stile_linea, LV_STATE_DEFAULT, true);

    //bool muri_oriz[N_CELLE*N_CELLE+N_CELLE];
    //bool muri_vert[N_CELLE*N_CELLE+N_CELLE];
    for (int i=0; i<N_CELLE*N_CELLE+N_CELLE; i++) {
        muri_vert[i] = true;
        muri_oriz[i] = true;
    }

    int destinazione=0;
    generate_maze(motionController, muri_oriz, muri_vert, &destinazione);
    cella_destinazione = destinazione;

    //disegna la destinazione
    lv_style_init(&stile_destinazione);
    lv_style_set_bg_color(&stile_destinazione, LV_STATE_DEFAULT, LV_COLOR_GREEN);
    destination = lv_obj_create(lv_scr_act(),nullptr);
    lv_obj_set_size(destination,CELL_SIZE,CELL_SIZE);
    lv_obj_add_style(destination, LV_OBJ_PART_MAIN, &stile_destinazione);
    lv_obj_set_pos(destination,X(destinazione)*CELL_SIZE,Y(destinazione)*CELL_SIZE);

    n_muri = 2;
    for (int i=0; i<N_CELLE; i++) {
        bool is_muro = false;
        for (int j=0; j<N_CELLE; j++) {
            if (!is_muro && muri_oriz[XY(j,i)]) {
                is_muro = true;
                n_muri++;
            }
            if (!muri_oriz[XY(j,i)]) {
                is_muro = false;
            }
        }
        if (is_muro) n_muri++;
    }
    for (int i=0; i<N_CELLE; i++) {
        bool is_muro = false;
        for (int j=0; j<N_CELLE; j++) {
            if (!is_muro && muri_vert[XY(i,j)]) {
                is_muro = true;
                n_muri++;
            }
            if (!muri_vert[XY(i,j)]) {
                is_muro = false;
            }
        }
        if (is_muro) n_muri++;
    }
    punti = new lv_point_t[n_muri][2];

    int index_muro = 0;
    for (int i=0; i<N_CELLE; i++) {
        bool is_muro = false;
        for (int j=0; j<N_CELLE; j++) {
            if (!is_muro && muri_oriz[XY(j,i)]) {
                is_muro = true;
                punti[index_muro][0].x = j*CELL_SIZE;
                punti[index_muro][0].y = i*CELL_SIZE;
            }
            if (is_muro && !muri_oriz[XY(j,i)]) {
                is_muro = false;
                punti[index_muro][1].x = j*CELL_SIZE;
                punti[index_muro][1].y = i*CELL_SIZE;
                //crea muro
                crea_muro(stile_linea, punti[index_muro]);
                index_muro++;
            }
        }
        if (is_muro) {
            punti[index_muro][1].x = N_CELLE*CELL_SIZE;
            punti[index_muro][1].y = i*CELL_SIZE;
            crea_muro(stile_linea, punti[index_muro]);
            index_muro++;
        }
    }

    for (int i=0; i<N_CELLE; i++) {
        bool is_muro = false;
        for (int j=0; j<N_CELLE; j++) {
            if (!is_muro && muri_vert[XY(i,j)]) {
                is_muro = true;
                punti[index_muro][0].x = i*CELL_SIZE;
                punti[index_muro][0].y = j*CELL_SIZE;
            }
            if (is_muro && !muri_vert[XY(i,j)]) {
                is_muro = false;
                punti[index_muro][1].x = i*CELL_SIZE;
                punti[index_muro][1].y = j*CELL_SIZE;
                //crea muro
                crea_muro(stile_linea, punti[index_muro]);
                //
                index_muro++;
            }
        }
        if (is_muro) {
            punti[index_muro][1].x = i*CELL_SIZE;
            punti[index_muro][1].y = N_CELLE*CELL_SIZE;
            crea_muro(stile_linea, punti[index_muro]);
            index_muro++;
        }
    }
    punti[index_muro][0].x = 239;
    punti[index_muro][0].y = 0;
    punti[index_muro][1].x = 239;
    punti[index_muro][1].y = 239;
    crea_muro(stile_linea, punti[index_muro]);
    index_muro++;
    punti[index_muro][0].x = 0;
    punti[index_muro][0].y = 239;
    punti[index_muro][1].x = 239;
    punti[index_muro][1].y = 239;
    crea_muro(stile_linea, punti[index_muro]);
    

    //lv_obj_t *linea = lv_line_create(lv_scr_act(), nullptr);
    //lv_obj_add_style(linea, LV_LINE_PART_MAIN, &stile_linea);
    //lv_obj_align(linea, nullptr, LV_ALIGN_IN_TOP_LEFT, 0,0);
    //lv_line_set_points(linea, punti_tmp, 2);

    lv_style_init(&stile_ball);
    lv_style_set_bg_color(&stile_ball, LV_STATE_DEFAULT, LV_COLOR_GREEN);

    ball = lv_obj_create(lv_scr_act(),nullptr);
    lv_obj_set_size(ball, BALL_SIZE,BALL_SIZE);
    lv_obj_add_style(ball, LV_OBJ_PART_MAIN, &stile_ball);
    //lv_obj_set_pos(ball,15,15);

    pos[0] = CELL_SIZE/2;
    pos[1] = CELL_SIZE/2;
    vel[0] = 0.;
    vel[1] = 0.;

    taskRefresh = lv_task_create(RefreshTaskCallback, LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID, this);
}

Maze::~Maze() {
    lv_style_reset(&stile_linea);
    lv_style_reset(&stile_destinazione);
    lv_style_reset(&stile_ball);
    lv_obj_clean(lv_scr_act());
    lv_task_del(taskRefresh);

    delete [] punti;
}

int round_f(float n) {
    return ((n-(int)n)>=0.5)?(int)n:(int)n +1;
}

void Maze::Refresh() {
    const float CELL_SIZE = 240./N_CELLE;
    vel[0] += motionController.X()/250.;
    vel[1] -= motionController.Y()/250.;;
    vel[0] *= 0.9;
    vel[1] *= 0.9;

    int cella = XY((int)(pos[0]/CELL_SIZE),(int)(pos[1]/CELL_SIZE));

    pos[0]+=vel[0];
    pos[1]+=vel[1];

    if (muri_vert[cella] && (pos[0]<X(cella)*CELL_SIZE+5)) {
        vel[0] *= -1;
        pos[0] = X(cella)*CELL_SIZE+5;
    }
    if (muri_vert[cella+1] && pos[0]>(X(cella)+1)*CELL_SIZE-5) {
        vel[0] *= -1;
        pos[0] = (X(cella)+1)*CELL_SIZE-5;
    }
    if (muri_oriz[cella] && (pos[1]<Y(cella)*CELL_SIZE+5)) {
        vel[1] *= -1;
        pos[1] = Y(cella)*CELL_SIZE+5;
    }
    if (muri_oriz[cella+N_CELLE] && pos[1]>(Y(cella)+1)*CELL_SIZE-5) {
        vel[1] *= -1;
        pos[1] = (Y(cella)+1)*CELL_SIZE-5;
    }
    
    lv_obj_set_pos(ball,round_f(pos[0])-5,round_f(pos[1])-5);

    if (!won && cella==cella_destinazione) {
        won = true;
        scritta = lv_label_create(lv_scr_act(), nullptr);
        lv_label_set_recolor(scritta,true);
        lv_label_set_text_static(scritta, "#00FF00 You Won!#");
        lv_label_set_align(scritta, LV_LABEL_ALIGN_CENTER);
        lv_obj_align(scritta, lv_scr_act(), LV_ALIGN_CENTER, 0, 0);   
    }
    if (won && cella!=cella_destinazione) {
        won = false;
        lv_obj_del(scritta);
    }
}

