//
// Created by Ankush J on 9/29/20.
//
const int num_ranks = 512;

const float oob_data[] = {
    3.94872189,
    4.32432556,
    3.67673683,
    4.81461382,
    3.77893186,
    8.61138344,
    5.76137972,
    5.10011959,
    5.95440626
};

const int oob_data_sz = 9;

const float range_min = -3.00000011e-06;
const float range_max = 2.21989846;

const int num_pivots = 256;

const uint64_t rank_bin_counts[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1};

const float rank_bins[] = {
    -3.00000011e-06, 0.00255585648, 0.00511323195, 0.00767006958, 0.0101526976,
    0.0124647999,    0.0147769023,  0.016894687,   0.0188452695,  0.0207195878,
    0.0225240719,    0.0241474062,  0.0257701464,  0.027792817,   0.0298225749,
    0.0321064442,    0.0343903154,  0.0366336443,  0.0387981161,  0.040962588,
    0.0423518941,    0.0436918326,  0.0451942272,  0.0474605784,  0.0497269295,
    0.0520080477,    0.0543020964,  0.056596145,   0.0586812645,  0.0607198849,
    0.0626810715,    0.0646023452,  0.066348277,   0.0679984018,  0.0690543652,
    0.0702383295,    0.0716654509,  0.0732882172,  0.0748690367,  0.0764549002,
    0.0780596063,    0.0800358281,  0.0821479559,  0.0841878355,  0.0861352533,
    0.0879650936,    0.0896377414,  0.091356732,   0.0930722132,  0.0941317454,
    0.0956776962,    0.0974306762,  0.0996664912,  0.101902314,   0.104273096,
    0.106731422,     0.109189749,   0.111580551,   0.11384628,    0.116112009,
    0.118064463,     0.119889542,   0.121141784,   0.122532174,   0.124075696,
    0.125676408,     0.12689586,    0.127894193,   0.129241034,   0.131229669,
    0.133218303,     0.135486692,   0.137775257,   0.140077621,   0.142425522,
    0.144773439,     0.147055969,   0.149222791,   0.151389599,   0.153102726,
    0.154612899,     0.155707702,   0.157696083,   0.159687459,   0.161968499,
    0.164249539,     0.166185901,   0.167430118,   0.169080704,   0.170384631,
    0.171277508,     0.172531083,   0.173627898,   0.174778387,   0.176131159,
    0.177667677,     0.178940594,   0.180570275,   0.182409465,   0.184009567,
    0.185687959,     0.187825397,   0.189962834,   0.191772714,   0.193577468,
    0.195614383,     0.197651297,   0.19932422,    0.200730324,   0.201545328,
    0.202318251,     0.203413203,   0.205134168,   0.20680517,    0.208045721,
    0.209585637,     0.211228192,   0.213012844,   0.214743584,   0.21641618,
    0.218502164,     0.220682979,   0.222922936,   0.225260586,   0.22759825,
    0.229841188,     0.232002348,   0.234121233,   0.235986575,   0.237761736,
    0.239264205,     0.241108894,   0.243073732,   0.244884625,   0.246630445,
    0.248777375,     0.250942856,   0.253088087,   0.255216956,   0.257212818,
    0.2587533,       0.260201395,   0.261538237,   0.262675554,   0.264385939,
    0.26617232,      0.268139333,   0.270059824,   0.271844804,   0.273382097,
    0.274675727,     0.276235193,   0.277824134,   0.279434085,   0.281124681,
    0.282821745,     0.284530312,   0.286018491,   0.287271291,   0.288242429,
    0.289511114,     0.290970623,   0.292370498,   0.293219775,   0.294341832,
    0.295823216,     0.297317743,   0.298704177,   0.300102323,   0.301507235,
    0.30291754,      0.304305196,   0.305606306,   0.306815118,   0.307945758,
    0.309287697,     0.310407251,   0.31144616,    0.312692523,   0.314403027,
    0.316481262,     0.31883651,    0.321191788,   0.323535293,   0.325873137,
    0.32821101,      0.330163926,   0.332040727,   0.334042937,   0.336055338,
    0.338300347,     0.340599328,   0.342851847,   0.344588876,   0.345972627,
    0.346955985,     0.348566711,   0.350002229,   0.351230383,   0.352362752,
    0.354591787,     0.356820852,   0.359058261,   0.361301005,   0.363534957,
    0.365499467,     0.367463976,   0.369212151,   0.370869339,   0.372251391,
    0.373541325,     0.375241637,   0.377373785,   0.379459798,   0.381311953,
    0.383093804,     0.384701759,   0.386709094,   0.388900459,   0.390954524,
    0.392534912,     0.394201368,   0.395914733,   0.397355974,   0.399164796,
    0.401371211,     0.403577626,   0.405421436,   0.407117754,   0.407912284,
    0.40936026,      0.411140263,   0.413129538,   0.415087819,   0.417019427,
    0.419041306,     0.421136886,   0.423177302,   0.425081879,   0.426800787,
    0.428160131,     0.429134965,   0.430304646,   0.431545049,   0.432672173,
    0.433242023,     0.434228182,   0.435622603,   0.436762422,   0.437805682,
    0.438980371,     0.440388739,   0.442145199,   0.443994462,   0.446023881,
    0.448069751,     0.449975163,   0.451832712,   0.453603506,   0.455357015,
    0.457080483,     0.458931625,   0.460991144,   0.463052869,   0.465124249,
    0.4671956,       0.468656659,   0.469809055,   0.470815182,   0.472024947,
    0.473262221,     0.474999219,   0.476870209,   0.478816181,   0.48077178,
    0.481695026,     0.482711345,   0.483832717,   0.485016763,   0.486271471,
    0.488175184,     0.49005121,    0.491271943,   0.492916852,   0.494709074,
    0.497069597,     0.499465883,   0.501862228,   0.504017115,   0.506148934,
    0.508316457,     0.510509968,   0.512619376,   0.514083087,   0.515676737,
    0.517343879,     0.518984854,   0.52075392,    0.522770643,   0.524891555,
    0.527379394,     0.529867172,   0.532355011,   0.534523129,   0.536632478,
    0.538645148,     0.54058075,    0.542579293,   0.544626474,   0.546627223,
    0.548555255,     0.550657272,   0.553011358,   0.555365443,   0.557769835,
    0.560221136,     0.562672436,   0.565006793,   0.567053735,   0.568957329,
    0.569786072,     0.571017206,   0.572575033,   0.574279547,   0.576469898,
    0.57866025,      0.581020474,   0.583504081,   0.585987687,   0.588501751,
    0.591123641,     0.59374553,    0.596367359,   0.598987877,   0.601600885,
    0.604213953,     0.606827021,   0.609436095,   0.612022936,   0.614609778,
    0.61719662,      0.619732261,   0.622145653,   0.624559045,   0.62691164,
    0.629032195,     0.631152689,   0.633161485,   0.635157466,   0.637351453,
    0.639591694,     0.641812444,   0.643970191,   0.646127939,   0.648407578,
    0.650708854,     0.652993083,   0.655145228,   0.657297373,   0.659280419,
    0.66120398,      0.663220525,   0.665265858,   0.66705507,    0.668669522,
    0.670832872,     0.673004687,   0.675365031,   0.677868724,   0.680372417,
    0.682888448,     0.685492635,   0.688096881,   0.690701067,   0.693213165,
    0.6952672,       0.697321296,   0.69937259,    0.701423824,   0.703244269,
    0.705006003,     0.706716895,   0.708304226,   0.709636569,   0.71181643,
    0.714041293,     0.716337562,   0.718735814,   0.721134067,   0.723510981,
    0.725852609,     0.728194177,   0.730380714,   0.732426643,   0.734310567,
    0.735898495,     0.737971008,   0.740188837,   0.742449105,   0.744838119,
    0.747227132,     0.749570489,   0.751773059,   0.753975689,   0.755777776,
    0.757479012,     0.759447873,   0.761428058,   0.763645768,   0.765863419,
    0.768152714,     0.77050674,    0.772860825,   0.775316119,   0.777829707,
    0.780343294,     0.782863677,   0.785423934,   0.787984192,   0.79054451,
    0.793114007,     0.795695722,   0.798277497,   0.800859272,   0.803380311,
    0.805860996,     0.808341682,   0.810813665,   0.813260019,   0.815706432,
    0.818148017,     0.820486009,   0.822824001,   0.825147569,   0.827414393,
    0.829681277,     0.831971586,   0.834280193,   0.83658886,    0.838733792,
    0.840838969,     0.843263626,   0.845977664,   0.848691761,   0.851405799,
    0.854119897,     0.856825531,   0.859529138,   0.862232745,   0.864936352,
    0.867631733,     0.870287597,   0.872943461,   0.875599325,   0.878255188,
    0.880900443,     0.883544445,   0.886188447,   0.88883245,    0.89146632,
    0.8940956,       0.896724939,   0.899354219,   0.901889145,   0.904356539,
    0.906823933,     0.909227669,   0.911476135,   0.913724601,   0.915843725,
    0.917906165,     0.919760108,   0.921401501,   0.922596097,   0.924084067,
    0.925799847,     0.92808485,    0.930430889,   0.932783306,   0.935300887,
    0.937818468,     0.940336049,   0.94288969,    0.945475459,   0.948061228,
    0.950647056,     0.953173161,   0.955671012,   0.958168805,   0.960716724,
    0.963407815,     0.966098964,   0.968790054,   0.971481144,   0.974228859,
    0.97699821,      0.979767501,   0.982536852,   0.985306203,   0.988112628,
    0.990956426,     0.993800998,   1.00020564,    1.0483973,     1.10408378,
    1.12131536,      1.14569116,    1.18038785,    1.22409606,    1.30815125,
    1.3920213,       1.66161156,    2.21989846};