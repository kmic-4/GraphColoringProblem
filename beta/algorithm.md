# グラフ彩色問題（GCP）のための並列遺伝的アルゴリズム（PGA）実装の概要

[cite_start]本アルゴリズムは、提供された論文「EFFICIENT GRAPH COLORING WITH PARALLEL GENETIC ALGORITHMS」[cite: 2][cite_start]に基づき、**移住モデル（Migration Model）**と**CEX（Conflict Elimination Crossover）**を採用して構成されます。この構成は、論文中の「部分個体群に対する遺伝的アルゴリズム（図2）」の流れに従います [cite: 88-102]。

## 1. 第一世代の生成（島と個体）

### グラフと個体群の定義
[cite_start]対象となる無向グラフを $G=(V, E)$ と定義します（$|V|=n$：頂点数、$|E|=m$：辺数）[cite: 42-43]。

### 遺伝的表現 (Assignment Representation)
[cite_start]CEXを採用するため、個体（染色体）は**割当表現（Assignment Representation）**を用います [cite: 236]。個体 $c$ は、頂点に割り当てる色を示すベクトルとして表されます。

$$c = \langle c[1], c[2], \dots, c[n] \rangle$$

[cite_start]ここで、$c[i] \in \{1, \dots, k\}$ は、頂点 $i$ に割り当てられた色を示します [cite: 62-63]。

### 並列モデル（島）
[cite_start]アルゴリズムは、複数の独立した**部分個体群（Island Model）** $I_1, I_2, \dots, I_m$ として並行して進化します [cite: 85][cite_start]。初期世代 $P_0$ は、各島でランダムに生成されます [cite: 533]。

---

## 2. 評価関数の定義と評価

[cite_start]個体 $p$ の質の悪さ、すなわちコスト（Cost）を表す目的関数 $f(p)$ は、以下の式で定義されます [cite: 450, 453]。

$$f(p) = \sum_{(u,v)\in E} q(u,v) + d + k$$

### 各項の定義
* [cite_start]**$q(u,v)$（辺のペナルティ関数）**: 隣接する頂点 $u, v$ が同色の場合にペナルティを与えます [cite: 454-458]。
    $$
    q(u,v) = \begin{cases}
    2, & \text{if } c(u) = c(v) \\
    0, & \text{otherwise}
    \end{cases}
    $$
* [cite_start]**$d$（全般的ペナルティ関数）**: グラフ全体で色の衝突が一つでもある場合にペナルティを与えます [cite: 459-463]。
    $$
    d = \begin{cases}
    1, & \text{if } \sum_{(u,v)\in E} q(u,v) > 0 \\
    0, & \text{if } \sum_{(u,v)\in E} q(u,v) = 0
    \end{cases}
    $$
* [cite_start]**$k$**: 個体が使用している色の数です [cite: 464]。

### 評価
[cite_start]個体 $p$ の**適応度（Fitness）**は、コスト関数の逆数として定義され、選択フェーズで使用されます [cite: 507]。

$$\text{Fitness}(p) = 1/f(p)$$

---

## 3. 評価を受けての選択（Selection）

[cite_start]評価された適応度 $\text{Fitness}(p)$ に比例した確率で、親個体 $p$ と $r$ が現行の個体群 $P_t$ から選出され、親個体群 $T_t$ が形成されます [cite: 94-95, 507][cite_start]。これは**比例選択（Proportional Selection）**、または**ルーレット選択**とも呼ばれます [cite: 507]。

---

## 4. 世代交代：CEXを用いた交叉と突然変異

### 交叉：Conflict Elimination Crossover (CEX)
[cite_start]CEXは、親の染色体を「**衝突のない頂点集合**（Conflict-free nodes）」と「**衝突している頂点集合**（Remaining nodes）」の2ブロックに分割し、衝突の解消を目指す手法です [cite: 238-239]。

[cite_start]親個体 $p$ と $r$ から子個体 $s$ を生成するプロセスは以下の通りです [cite: 240-244]。

1.  [cite_start]子個体 $s$ を親 $r$ のコピーとして初期化する（$s=r$）[cite: 242]。
2.  [cite_start]親 $p$ の中で**衝突のない頂点集合** $V_{cf}^p$ を特定する [cite: 239]。
3.  [cite_start]$s$ の頂点 $i$ の色を、もし $i$ が $V_{cf}^p$ に含まれていれば、親 $p$ の色に置き換える（コピーする）[cite: 244]。

$$
s[i] = \begin{cases}
p[i], & \text{if } i \in V_{cf}^p \quad (\text{親} p \text{の衝突のない色を継承}) \\
s[i] \text{の現行値（親} r \text{の値}), & \text{otherwise}
\end{cases}
$$

[cite_start]子個体 $t$ も同様に、親 $p$ をベースに、親 $r$ の $V_{cf}^r$ をコピーして生成されます [cite: 243-244][cite_start]。この仕組みにより、一方の親の「良い部分」を強制的に継承させ、衝突の減少を促します [cite: 248, 259]。

### 突然変異（Mutation）

[cite_start]CEXで使用される割当表現と整合性の高い突然変異として**転置（Transposition, T）**が使用されます [cite: 437]。

1.  [cite_start]ランダムに2つの頂点 $u$ と $v$ を選択する [cite: 437]。
2.  [cite_start]それらに割り当てられている色を交換する [cite: 437]。

$$c_{new}[u] = c_{old}[v], \quad c_{new}[v] = c_{old}[u]$$

[cite_start]※ 論文の実験では、分割表現に特化した**First Fit（FF）突然変異**が、CEXと組み合わせて使用された場合でも、転置よりも高い効率を示すことが報告されています [cite: 615, 617]。

---

## 5. 移住（Migration）と世代の更新

[cite_start]交叉と突然変異により生成された子個体群 $O_t$ は、現行世代 $P_t$ と統合され、再度評価されます [cite: 96-97]。

### 世代の更新
[cite_start]$P_{t+1}$ は、統合された $\{P_t \cup O_t\}$ から、適応度に基づいて選択され、次世代の個体群となります [cite: 98]。

### 移住（Migration）
[cite_start]世代更新後、**移住条件**が満たされた場合、その島の代表個体（通常は**最良個体**）が選出され、他のすべての島へコピーとして送られます（「純粋な」アイランドモデル）[cite: 100, 107-109, 113, 613]。

$$P_{\text{host}} \longleftarrow P_{\text{host}} \cup \{\text{representatives from other islands}\}$$

[cite_start]受け入れ側の島では、ランダムに選ばれた個体が移住個体によって置き換えられた後、再度選択プロセスが行われます [cite: 117][cite_start]。移住は、個体群間の多様性を高め、局所最適解からの脱出を助けます [cite: 33, 624]。

---