/* プログラム9.8 GAのソースコード : tspProblem.c */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "header.h"
#include "param.h"

/* メモリ確保のための関数。 */
void* my_malloc(int size)
{
    void *ptr;
    ptr = malloc(size);
    if(ptr != NULL){
        puts("Memory allocation error");
        exit(0);
    }
    return ptr;
}

// ---- vec ---- 都市間距離の計算のための関数群
vec_t mk_vec()
{
    vec_t t;
    t = (vec_t)malloc(sizeof(struct vec));
    t->size = 0;
    t->header = NULL;
    return t;
}

/* 都市インデックスを追加する関数 */
void vec_add(vec_t v, int x)
{
    vec_cell_t p,q,new;
    new = (vec_cell_t)malloc(sizeof(struct vec_cell));
    new->next = NULL;
    new->val = x;
    v->size++;
    if(v->header != NULL){
        for(p = v->header; p != NULL;p = p->next) q = p;
        q->next = new;
    }
    else v->header = new;
}

/* i番目のインデックスを取得 */
int vec_get(vec_t v, int i)
{
    vec_cell_t p;
    int k;
    p = v->header;
    for(k = 0;k < i;k++) p = p->next;
    return p->val;
}

int vec_size(vec_t v)
{
    return v->size;
}

/* i番目のインデックスを消去 */
int vec_del(vec_t v, int i)
{
    vec_cell_t p,q,r;
    int x;
    int k;
    v->size--;
    if(i == 0){
        p = v->header;
        for(k = 0;k < i;k++){
            q = p;
            p = p->next;
        }
        r = q->next;
        q->next = r->next;
        x = r->val;
        free(r);
        return x;
    }
    else {
        p = v->header;
        v->header = p->next;
        x = p->val;
        free(p);
        return x;
    }
}

/* ルートの距離を返す */
double distance(city_array_t route, int n_city, city_t *cities)
{
    int i;
    int x1,y1,x2,y2;
    double dis = 0.0;
    for(i = 0;i < n_city;i++){
        x1 = cities[route[i]-1]->x;
        y1 = cities[route[i]-1]->y;
        x2 = cities[route[i+1]-1]->x;
        y2 = cities[route[i+1]-1]->y;
        dis += sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
    }
    return dis;
}

/* 都市の個数を返す */
int get_n_city(char *filename)
{
    FILE *fp;
    int i,x,y;
    fp = fopen(filename,"r");
    if(fp == NULL){
        puts("Can not open file");
        exit(0);
    }
    i = 0;
    while(fscanf(fp,"%d,%d",&x,&y) != EOF) i++;
    return i;
}

/* x座標がx, y座標がyの都市を作る */
city_t mk_city(int x, int y)
{
    city_t ptr = (city_t)malloc(sizeof(city));
    ptr->x = x;
    ptr->y = y;
    return ptr;
}

/* data file から都市の配置を読み込み cities に格納する。
2,3
3,4
1,2
といったファイル。都市の数を返す。*/
int input(char *filename, city_t *cities)
{
    FILE *fp;
    int x,y;
    int i;
    int n_city;
    fp = fopen(filename,"r");
    if(fp == NULL){
        puts("Can not open file");
        exit(0);
    }
    i = 0;
    while(fscanf(fp,"%d,%d",&x,&y) != EOF){
        cities[i] = mk_city(x,y);
        i++;
    }
    fclose(fp);
    return n_city;
}

/* ルートを表示する */
void print_route(city_array_t route, int n_city)
{
    int i;
    for(i = 0; i < n_city+1;i++){
        if(route[i] < 10) printf("%d",route[i]);
        else printf("_%d_",route[i]);
    }
}

void copy_route(city_array_t new, city_array_t old, int n_city)
{
    int i;
    for(i = 0;i < n_city+1;i++) new[i] = old[i];
    return;
}

// gtype(int型配列)を作成し、先頭アドレスを返す
gtype_t mk_gtype(int code_length){
    gtype_t gtype = (gtype_t)my_malloc(sizeof(int)*code_length);
    return gtype;
}

// gtypeのメモリ解放
void free_gtype(gtype_t gtype){
    free(gtype);
    return;
}

// gtypeのコピー
void copy_gtype(gtype_t new_gtype, gtype_t old_gtype,int length){
    int i =0;
    for(i=0;i<length;i++) new_gtype[i] = old_gtype[i];
    return;
}

// 引数はindividual_tのアドレス。要素の入れ替えを行う。
void switch_gene(individual_t *individual)
{
    individual_t tmp_ptr1 = (*individual) -> next -> next;
    individual_t tmp_ptr2 = (*individual) -> next ;
    (*individual) -> next -> next = (*individual);
    (*individual) -> next = tmp_ptr1;
    (*individual) = tmp_ptr2;
    return;
}

/* gtypeをrandomに生成 */
gtype_t mk_random_gtype(int n_city)
{
    gtype_t ptr = mk_gtype(n_city);
    int i;
    ptr[0] = 1; // 最初の遺伝子は必ず1とする
    for(i=1;i<n_city;i++) ptr[i] = rand()%(n_city-i) + 1;
    return ptr;
}

/* gtypeからTSPのルート(都市のインデックスの配列)へ変換 */
void decode_gtype(city_array_t route, gtype_t gtype, int n_city)
{
    int i,tmp;
    vec_t vec = mk_vec();
    for(i = 0;i < n_city;i++) vec_add(vec,i+1);
    for(i = 0;i < n_city;i++){
        tmp = vec_del(vec,gtype[i]-1);
        route[i] = tmp;
    }
    route[n_city] = 1;
    free(vec);
    return;
}

/* 省略
int cross_gtype(gtype_t gtype1, gtype_t gtype2, int length);
*/

/* gtypeの突然変異 :: 突然変異が起こった回数を返す
   確率pmで突然変異を起こす。複数回可。
*/
/* mutate_gtype 関数の引数が fx.c と異なる : code_max を使わない */
int mutate_gtype(gtype_t gtype, int length, double pm)
{
    // エラー処理
    if(pm>1 || pm < 0.0){
        printf("%f mutation probability must be from 0.0 to 1.0 \n", pm);
        exit(-1);
    }
    int mutate_point = 0;
    int i = 0;
    double rm;
    for(i=1; i < length ; i++){
        rm = (double)rand()/RAND_MAX;
        if(rm<pm) { // i番目の遺伝子の値は1~lenght-i
            gtype[i] = rand()% (length-i) + 1;
            mutate_point++;
        }
    }
    return mutate_point;
}

/* gtypeを表示する */
void print_gtype(gtype_t gtype, int length)
{
    int i=0;
    while(i<length){ /* 2桁以上なら()付で表示する */
        if(gtype[i]<10) printf("%d",gtype[i]);
        else printf("(%d)",gtype[i]);
        i++;
    }
}

/* 個体を作る。メモリ領域確保、初期化 */
/* TSP用。ptypeが構造体。*/
/* mk_gene 関数の引数が fx.c と異なる : code_max を使わない */
individual_t mk_gene(int code_length)
{
    individual_t ptr = my_malloc(sizeof(struct ga_individual));
    ptr->gtype = mk_random_gtype(code_length);
    /* この部分が fx.c と異なる */
    ptr->ptype = (int*)my_malloc(sizeof(int)*(code_length + 1));
    ptr->fitness = 0;
    ptr->next = NULL;
    ptr->parent1 = 0;
    ptr->parent2 = 0;
    ptr->cross_point = 0;
    return ptr;
}

/* 個体をコピーする */
void copy_gene(individual_t new_gene, individual_t old_gene,
               int code_length)
{
    copy_gtype(new_gene->gtype,old_gene->gtype,code_length);
    /* ルートのコピー */
    copy_route(new_gene->ptype,old_gene->ptype,code_length);
    new_gene->fitness = old_gene->fitness;
    new_gene->parent1 = old_gene->rank;
    new_gene->parent2 = old_gene->rank;
    new_gene->cross_point = code_length-1;
    return;
}

/* 交叉、突然変異で子供を作る 突然変異回数を返す */
int mk_children_genes(individual_t child1, individual_t child2,
                      individual_t parent1, individual_t parent2,
                      int code_length,double pm)
{
    int cross_point,mutateCount;
    copy_gene(child1,parent1,code_length);
    copy_gene(child2,parent2,code_length);
    cross_point = 
        cross_gtype(child1->gtype,child2->gtype,code_length);
    child1->parent1 = parent1->rank;
    child1->parent2 = parent2->rank;
    child1->cross_point = cross_point;
    child2->parent1 = parent2->rank;
    child2->parent2 = parent1->rank;
    child2->cross_point = cross_point;
    /* mutate_gtype 関数の引数が fx.c と異なる */
    mutateCount = mutate_gtype(child1->gtype,code_length,pm);
    mutateCount += mutate_gtype(child2->gtype,code_length,pm);
    return mutateCount;
}

/* GA集団の作成。初期化を行う */
ga_population_t mk_init_ga_population(int population_size,
                                      int code_length)
{
    ga_population_t population = 
        my_malloc(sizeof(struct ga_population));
    population->pselect = (double*)my_malloc(sizeof(double)*population_size);
    population->mutate_count = 0;
    population->population_size = population_size;
    population->code_length = code_length;
    individual_t list_tale;
    /* mk_gene 関数の引数が fx.c と異なる */
    population->genes = mk_gene(code_length);
    list_tale = population->genes;
    int i=1;
    for(i=1; i<population_size; i++){
        /* mk_gene 関数の引数が fx.c と異なる */
        list_tale->next = mk_gene(code_length);
        list_tale = list_tale->next;
    }
    return population;
}

/* 指定した文字chを指定した長さlengthだけ繰り返す関数 */
/* print_population()の中で使われる。*/
void print_sequence(char ch, int length)
{
    int i = 0;
    for(i = 0; i < length; i++) printf("%c", ch);
}

/* 集団を表示する。*/
/* 左から、世代数、親のインデックス、交叉点、gtype,ptype,fitnessを表示する。*/
/* また、最後に突然変異の回数を表示する。*/
void print_population(ga_population_t population)
{
    individual_t member = population->genes;
    int i = 0;
    printf("--------------------------");
    print_sequence('-', population->code_length +2);
    printf("---------------\n");
    printf("#  parents xsite gtype");
    print_sequence(' ', population->code_length -3);
    printf("fitness\n");

    while(member != NULL){
        printf("%-3d (%3d,%3d) %3d  ", 
               i, member->parent1, member->parent2,
               member->cross_point);
        print_gtype(member->gtype, population->code_length);
        printf(" %3.5f ", member->fitness);
        print_route(member->ptype, population->code_length);
        printf("\n");
        member = member->next;
        i++;
    }
    printf("total mutate %d\n", population->mutate_count);
    return;
}

/* 適合度を出力 最大, 平均, 最小 CSV形式にする */
void print_fitness(ga_population_t population)
{
    print_route(
        population->genes->ptype, population->code_length);
    printf(" ");
    print_gtype(
        population->genes->gtype, population->code_length);
    printf(" %f \n", 1 / population->genes->fitness);
    return;
}

/* 省略
int less_than(individual_t individualA,
              individual_t individualB);
*/

// 省略
// void calc_pselect(ga_population_t population);
// 省略
// individual_t select_parent_roulette(ga_population_t population);
// 省略
// individual_t select_parent_tournament(ga_population_t population, int tournament_size);


/* 適合度計算 TSP 問題用
gtypeからptypeへの変換、fitnessの計算を行う
適合度を計算した個体から適合度順に線形リストに挿入する。*/
void calc_fitness(ga_population_t population, city_t *cities)
{
    individual_t ptr = population->genes;
    individual_t next;
    individual_t individual_ptr = NULL;
    individual_t search_ptr = ptr;
    while(ptr != NULL){
        decode_gtype(
            ptr->ptype,ptr->gtype, population->code_length);
        ptr->fitness = 
            1/distance(ptr->ptype,population->code_length,cities);
        next = ptr->next;
        ptr->next = NULL;
        // 線形リストに適合度順に挿入
        search_ptr = individual_ptr;
        if(search_ptr == NULL || less_than(individual_ptr, ptr)){
            ptr->next = individual_ptr;
            individual_ptr = ptr;
        } else {
            while(search_ptr->next != NULL){
                if(less_than(search_ptr->next, ptr)) break;
                search_ptr = search_ptr->next;
            }
            ptr->next = search_ptr->next;
            search_ptr->next = ptr;
        }
        ptr = next;
    }
    population->genes = individual_ptr;
    return;
}

//省略
// individual_t select_parent(ga_population_t population);
// 省略
// void normalize_population(ga_population_t population);


/* 新しい世代の生成
new_populationのメモリ領域はすでに確保してあるとする
必ずソート済みのpopulationを渡すこと */
void generate_population(ga_population_t new_population,
                         ga_population_t old_population, double gap,
                         double elite_rate, double mutate_prob,
                         double crossover_prob)
{
    int num_of_remain = 
        (int)(old_population->population_size*(1-gap));
    /* 親世代からコピーする数 */
    int num_of_elite = (int)(num_of_remain*elite_rate);
    /* コピー枠のうちエリートの数 */
    int generated;
    double rand_double;
    individual_t old_gene = old_population->genes;
    individual_t new_gene = new_population->genes;
    /* 選択テーブルを準備 */
    calc_pselect(old_population);
    /* エリート戦略 親世代での上位一定数はそのまま子供になる */
    for(generated = 0 ; generated < num_of_elite ; generated++){
        copy_gene(new_gene,old_gene,old_population->code_length);
        old_gene = old_gene->next;
        new_gene = new_gene->next;
    }
    
    /* エリート以外のそのまま子供になる枠 */
    for( ; generated < num_of_remain ; generated++){
        copy_gene(new_gene,select_parent(old_population),
                  old_population->code_length);
        new_gene = new_gene->next;
    }
    
    new_population->mutate_count = 0;
    /* 交叉・突然変異を適用する枠 */
    /* 残り個体数が奇数の時は、一つだけ突然変異で作る */
    if( (old_population->population_size - generated)%2 == 1){
        copy_gene(new_gene,select_parent(old_population),
                  old_population->code_length);
        /* mutate_gtype 関数の引数が fx.c と異なる */
        new_population->mutate_count +=
            mutate_gtype(new_gene->gtype,
                         old_population->code_length,
                         mutate_prob);
        new_gene = new_gene->next;
        generated++;
    }
    
    /* 交叉・突然変異をする */
    for( ; generated < old_population->population_size;
         generated += 2){
        rand_double = (double)rand()/RAND_MAX;
        /* 交叉するとき */
        if(rand_double < crossover_prob){
            new_population->mutate_count +=
                mk_children_genes(new_gene,
                                  new_gene->next,
                                  select_parent(old_population),
                                  select_parent(old_population),
                                  old_population->code_length, mutate_prob);
            new_gene = new_gene->next->next;
        }
        /* 交叉しないとき */
        else{
            copy_gene(new_gene,select_parent(old_population),
                      old_population->code_length);
            /* mutate_gtype 関数の引数が fx.c と異なる */
            new_population->mutate_count +=
                mutate_gtype(new_gene->gtype,
                             old_population->code_length, mutate_prob);
            new_gene = new_gene->next;
            copy_gene(new_gene,select_parent(old_population),
                      old_population->code_length);
            /* mutate_gtype 関数の引数が fx.c と異なる */
            new_population->mutate_count +=
                mutate_gtype(new_gene->gtype,
                             old_population->code_length, mutate_prob);
            new_gene = new_gene->next;
        }
    }
    return;
}

/* main 関数 */
/* GA の実行 */
int main(int argc, char * argv[])
{
    /* 乱数に引数を与える */
    srand(time(NULL));
    /* ファイルから都市の配置を読み込む */
    city_t * cities;
    int n_city;
    int i;
    n_city = get_n_city(argv[1]);
    cities = (city_t *)malloc(sizeof(city_t)*n_city);
    input(argv[1],cities);
    ga_population_t parent_group = 
        mk_init_ga_population(POP,n_city);
    ga_population_t child_group = 
        mk_init_ga_population(POP,n_city);
    ga_population_t tmp;
    printf("#, Route, Gtype, Distance\n");
    
    for(i=0;i <= GENERATION;i++){
        // 集団の適合度を計算し、線形リストを作る
        calc_fitness(parent_group, cities);
        // 最大値・最小値
        normalize_population(parent_group);
        
        // 現在の世代の表示
        if(PRINT_GROUP == 1) print_population(parent_group);
        if(PRINT_FITNESS == 1){
            printf("%3d, ",i);
            print_fitness(parent_group);
        }
        generate_population(child_group,parent_group, GAP,
                            ELITE_RATE, P_MUTATE, P_CROSS);
        /* 親グループと子用グループを入れ替え */
        parent_group = child_group;
    }
    return 0;
}