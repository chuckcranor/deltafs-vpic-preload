//
// Created by Ankush J on 11/12/20.
//

const int num_ranks = 512;

const float range_min = -0.000179999639;
const float range_max = 462.990967;
const int oob_data_sz = 0;
const float oob_data[] = {};

const int num_pivots = 256;

const float rank_bin_counts[] = {
    0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 5941, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,    0, 0, 0};

const float rank_bins[] = {
    -0.000179999639, 0.00856544357, 0.015529451,  0.0213691611, 0.0263514929,
    0.0309106596,    0.0352350473,  0.0392805263, 0.0431430675, 0.0468983576,
    0.0504962243,    0.054046154,   0.0575287119, 0.0609311834, 0.0642676353,
    0.0675764531,    0.0708028227,  0.0739648789, 0.0770870075, 0.0801636428,
    0.0831757709,    0.086128369,   0.0890762955, 0.0920187905, 0.0949438065,
    0.0978602469,    0.100777887,   0.103672229,  0.106507055,  0.109341137,
    0.112172723,     0.114995942,   0.117816128,  0.120589115,  0.123328827,
    0.126058325,     0.128802955,   0.131544262,  0.13426356,   0.137008369,
    0.137960866,     0.139313102,   0.142000303,  0.144704252,  0.147430167,
    0.150136501,     0.152830452,   0.155532002,  0.158212006,  0.160878941,
    0.163528606,     0.166191965,   0.16883564,   0.171464816,  0.174094304,
    0.176743433,     0.179398194,   0.182047293,  0.184687883,  0.187312946,
    0.187861785,     0.189235315,   0.1901467,    0.191369593,  0.194020152,
    0.196670249,     0.199321896,   0.201980293,  0.204602316,  0.207209051,
    0.209815696,     0.212452859,   0.215115145,  0.217779279,  0.220460087,
    0.223108709,     0.225726888,   0.228348434,  0.230978444,  0.233605474,
    0.236224547,     0.238851279,   0.241480425,  0.244138718,  0.246804044,
    0.249460161,     0.252103835,   0.254739374,  0.257390946,  0.260063767,
    0.262744576,     0.265428841,   0.268120587,  0.270818025,  0.273510814,
    0.276197135,     0.278913736,   0.281658888,  0.282402307,  0.284018904,
    0.286763042,     0.28954801,    0.292339802,  0.295148969,  0.297944635,
    0.300720662,     0.303489745,   0.306286663,  0.309120089,  0.31194818,
    0.314759463,     0.317582756,   0.320437312,  0.323354065,  0.326279759,
    0.329197288,     0.332133353,   0.335054815,  0.337995023,  0.340971142,
    0.343957305,     0.346942186,   0.349934101,  0.352956951,  0.355960518,
    0.358974725,     0.361992657,   0.365023494,  0.368066609,  0.371150911,
    0.374263853,     0.377403408,   0.380542755,  0.383724719,  0.386951536,
    0.39020887,      0.393461645,   0.396684736,  0.39992702,   0.403171808,
    0.406423151,     0.409689754,   0.412981093,  0.416344911,  0.419686943,
    0.420497686,     0.42316407,    0.426505059,  0.429867625,  0.43328467,
    0.436743289,     0.440228105,   0.443755925,  0.447265029,  0.450781733,
    0.454313189,     0.457881629,   0.46149841,   0.465139776,  0.468802184,
    0.47253263,      0.476237059,   0.479613006,  0.480795085,  0.481977165,
    0.48579064,      0.489651084,   0.493545592,  0.497040093,  0.499499619,
    0.499967068,     0.503661931,   0.50752455,   0.511413217,  0.515362263,
    0.519429922,     0.523522675,   0.527679026,  0.531935275,  0.536218703,
    0.540491939,     0.544784665,   0.549120724,  0.553571284,  0.558030665,
    0.562532723,     0.56710726,    0.571752906,  0.576439679,  0.581175923,
    0.584273577,     0.585329413,   0.588141382,  0.590143085,  0.590364397,
    0.590585828,     0.592512131,   0.597179055,  0.601876497,  0.60576576,
    0.606084704,     0.609625816,   0.614783049,  0.619977117,  0.625249028,
    0.630624294,     0.636103213,   0.64161849,   0.64722842,   0.650829434,
    0.652101576,     0.656234801,   0.661954939,  0.667835712,  0.673789859,
    0.678862631,     0.681187928,   0.682296097,  0.683407605,  0.684519112,
    0.688336909,     0.690681994,   0.693128586,  0.6994977,    0.705968559,
    0.712546349,     0.719321609,   0.725405872,  0.728165627,  0.730184138,
    0.736752748,     0.743445873,   0.746262491,  0.749152601,  0.755806565,
    0.761778057,     0.763940752,   0.767168522,  0.772668481,  0.775501907,
    0.780596614,     0.788746774,   0.797115088,  0.80568397,   0.814542055,
    0.823589146,     0.832080305,   0.840058684,  0.842664242,  0.850923598,
    0.860758603,     0.866606057,   0.86746788,   0.868330896,  0.869193912,
    0.874873877,     0.877073944,   0.883602738,  0.893785834,  0.89591819,
    0.895967066,     0.896015942,   0.896064878,  0.896113813,  0.896162748,
    0.896211624,     0.896260619,   0.896309555,  0.89635843,   0.896407366,
    0.896456361,     0.896505237,   0.896554112,  0.896603107,  0.899904907,
    0.905458331,     0.90905863,    0.921046555,  0.932544172,  0.940391004,
    0.941606998,     0.942823052,   0.944039166,  0.94525522,   0.948196828,
    0.95334959,      0.966952145,   0.98131454,   0.996321559,  1.01100993,
    1.01488173,      1.01624441,    1.01760685,   1.01944959,   1.02302456,
    1.0395999,       1.05473292,    1.05893767,   1.06000996,   1.06029677,
    1.06058383,      1.06087065,    1.06451082,   1.06696713,   1.06943476,
    1.07190418,      1.08916116,    1.1109395,    1.13194752,   1.1425432,
    1.14355659,      1.14457011,    1.15851438,   1.17363858,   1.18177009,
    1.20304143,      1.21388853,    1.21593499,   1.21798158,   1.22002804,
    1.22510219,      1.23246145,    1.24002063,   1.2496053,    1.26376367,
    1.27818251,      1.27858639,    1.27899027,   1.27939427,   1.29813695,
    1.32140958,      1.33199322,    1.34228361,   1.35534358,   1.36233211,
    1.36780572,      1.37327945,    1.38911796,   1.39733899,   1.4213866,
    1.4368161,       1.47219646,    1.48205245,   1.50393403,   1.555053,
    1.57348192,      1.59575605,    1.59742308,   1.59764636,   1.61471415,
    1.67719662,      1.69893384,    1.73329532,   1.738639,     1.75043404,
    1.76563895,      1.78285325,    1.84637487,   1.86693859,   1.86694384,
    1.86694932,      1.8669548,     1.86696005,   1.86696565,   1.86697102,
    1.86697638,      1.86698186,    1.86698723,   1.91194344,   1.98122323,
    1.99791443,      2.01460314,    2.02800703,   2.035465,     2.04292297,
    2.05038095,      2.06349182,    2.09196353,   2.10377121,   2.11455059,
    2.12533021,      2.15376759,    2.17737126,   2.22264791,   2.25495887,
    2.30126452,      2.33303905,    2.45072412,   2.54939342,   2.57064295,
    2.63480973,      2.6349082,     2.63500643,   2.63510466,   2.63520336,
    2.63530135,      2.63539982,    2.63549805,   2.63559651,   2.64434218,
    2.72860527,      2.73865771,    2.74871039,   2.75876307,   2.87452698,
    3.07278109,      3.1136272,     3.26157451,   3.41344786,   3.41665268,
    3.41985798,      3.45241213,    3.51202703,   3.67172194,   3.71260715,
    3.83424044,      3.85921431,    3.86154127,   3.86386847,   3.86625934,
    3.97409964,      4.00717449,    4.03676605,   4.04406738,   4.05136919,
    4.17385197,      4.19836378,    4.19994497,   4.24461269,   4.49361181,
    4.53690577,      4.56663418,    4.59637499,   4.68183899,   4.71351242,
    4.74518585,      4.79623413,    4.84745073,   4.97999048,   5.00374794,
    5.05345583,      5.10410261,    5.21617603,   5.28711653,   5.54421616,
    5.65217686,      5.74817657,    5.83433437,   5.92531633,   5.95901251,
    6.18691397,      6.39270449,    6.3968153,    6.40092611,   6.75797224,
    7.20134783,      7.29190779,    7.48335361,   7.60294628,   7.71884775,
    8.18355179,      8.2560358,     8.32932281,   8.42926693,   8.70670128,
    8.75708008,      9.38304901,    9.51179504,   9.54839516,   9.71290016,
    9.93721008,      10.210681,     10.8005915,   11.495203,    12.2137632,
    12.7449961,      12.8012819,    12.8575678,   13.3424044,   13.3574677,
    13.3725309,      14.2818937,    14.8833904,   14.9022188,   14.9210463,
    15.0514565,      15.2235403,    16.5002213,   17.1112041,   17.2607498,
    17.4102974,      17.7475872,    18.3879204,   18.6642876,   19.1667595,
    21.7319546,      24.8191738,    27.2245159,   27.8944664,   28.5644188,
    29.2579918,      32.0022736,    37.9064865,   46.0543594,   54.1026497,
    62.762989,       70.8352432,    79.4521408,   89.5121307,   121.948593,
    171.527359,      276.786591,    462.990967};