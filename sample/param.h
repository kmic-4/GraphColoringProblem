/* GAで使われるパラメータをまとめて記述するファイル */
#define POP 300       /* 個体数 */
#define GAP 0.95      /* 一回の生殖で子供と入れ替わる割合 */
#define ELITE_RATE 1.0 /* そのまま残る数のうち、エリートの割合 */
#define P_MUTATE 0.1  /* 突然変異率。LENGTHの逆数程度がよい */
#define P_CROSS 0.7   /* 交叉率 */
#define GENERATION 30 /* 最大世代数 */
#define SELECTION_METHOD 2 /* 1はルーレット 2はトーナメント */
/* トーナメントサイズ。トーナメントのときだけ意味がある */
#define TOURNAMENT_SIZE 5

/* 出力 */
#define PRINT_GROUP 0
#define PRINT_FITNESS 1