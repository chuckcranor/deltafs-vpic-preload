//
// Created by Ankush J on 9/18/20.
//

const int num_ranks = 512;

const float oob_data[] = {};
const int oob_data_sz = 0;

const float range_min = -0.000120000033;
const float range_max = 11.7258329;

const int num_pivots = 64;

const float rank_bin_counts[] = {
    0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 33637, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,     0, 0};

const float rank_bins[] = {
    -0.000120000033, 0.00326239085, 0.00664378144, 0.0100251725, 0.0134065636,
    0.0167879555,    0.0201693475,  0.0235507395,  0.0269119237, 0.0291843638,
    0.0312710777,    0.0333577916,  0.0354445055,  0.0375312194, 0.0396179333,
    0.0417046472,    0.0437754691,  0.0456384011,  0.0474101007, 0.0491802879,
    0.050950475,     0.0527206585,  0.0544908457,  0.0562610291, 0.0580192022,
    0.0596957952,    0.0613257103,  0.0629404113,  0.0645551085, 0.0661698133,
    0.0677845106,    0.0693992153,  0.0710025057,  0.0725580677, 0.0740917102,
    0.0756059662,    0.0771202222,  0.0786344782,  0.0801487342, 0.0816629902,
    0.0831684545,    0.0846443996,  0.0861067325,  0.087553665,  0.0889968053,
    0.0904399455,    0.0918830857,  0.0933262259,  0.0947614312, 0.0961763337,
    0.0975812674,    0.0989777148,  0.100365333,   0.101752952,  0.10314057,
    0.104528189,     0.105909199,   0.107276909,   0.10863971,   0.109998666,
    0.111351565,     0.112703219,   0.114054874,   0.115406536,  0.116753384,
    0.118090764,     0.119422488,   0.120749377,   0.122071773,  0.123391524,
    0.124711268,     0.126031011,   0.127345935,   0.128652915,  0.129955947,
    0.131257147,     0.132555917,   0.13385129,    0.135146528,  0.136441767,
    0.137732014,     0.139016598,   0.140298739,   0.141578987,  0.142857239,
    0.14413254,      0.145406708,   0.146680877,   0.147952363,  0.149221003,
    0.150484994,     0.151747972,   0.153009325,   0.154268906,  0.155528441,
    0.156787977,     0.158046082,   0.159302443,   0.160557464,  0.161812246,
    0.163066089,     0.164319232,   0.165570885,   0.166821897,  0.168072179,
    0.169322148,     0.170569479,   0.171816811,   0.173063934,  0.174309984,
    0.175554246,     0.176797152,   0.17803739,    0.179275662,  0.180511773,
    0.181747854,     0.182983175,   0.18421784,    0.185450822,  0.186682299,
    0.18791239,      0.189141586,   0.190371007,   0.191600457,  0.192829296,
    0.194057569,     0.195286691,   0.196515918,   0.197744653,  0.198973149,
    0.200201824,     0.201430485,   0.20265907,    0.203887463,  0.205115706,
    0.206343889,     0.207573503,   0.208803907,   0.210034221,  0.211264521,
    0.212494969,     0.213726655,   0.214958921,   0.216191694,  0.217424467,
    0.218657032,     0.219889596,   0.22112225,    0.222354904,  0.223587304,
    0.22481975,      0.226052403,   0.227286965,   0.228522196,  0.229758322,
    0.230994433,     0.232230544,   0.23346737,    0.234704599,  0.235941693,
    0.237179413,     0.238418385,   0.239657909,   0.240897983,  0.242138058,
    0.243377998,     0.244617745,   0.245857984,   0.247099429,  0.248341829,
    0.249585137,     0.250829369,   0.252073586,   0.253317922,  0.254562736,
    0.255807936,     0.257055342,   0.258304656,   0.259555101,  0.260806322,
    0.262057543,     0.263308793,   0.26456058,    0.265812755,  0.267067611,
    0.26832366,      0.269581079,   0.270840257,   0.272099406,  0.273358583,
    0.274618626,     0.275878966,   0.277142733,   0.278408408,  0.279676199,
    0.280946195,     0.282216191,   0.283486187,   0.284757853,  0.286030352,
    0.28730607,      0.288585037,   0.289865464,   0.291150987,  0.29243654,
    0.293722063,     0.29500854,    0.29629606,    0.297586799,  0.298882008,
    0.300177544,     0.3014763,     0.302775055,   0.304073811,  0.305372924,
    0.30667308,      0.30797568,    0.30928275,    0.31058988,   0.311901778,
    0.313214153,     0.314526498,   0.315839052,   0.317153484,  0.318469822,
    0.319791853,     0.321113974,   0.322441101,   0.32376954,   0.325097978,
    0.326426417,     0.327756733,   0.329089522,   0.330428332,  0.331767261,
    0.333113849,     0.334462732,   0.335811615,   0.337160468,  0.33851108,
    0.339864314,     0.341224849,   0.342586398,   0.343952239,  0.34532097,
    0.346689969,     0.348058969,   0.349429399,   0.350802183,  0.352181405,
    0.353562444,     0.354946136,   0.35633269,    0.357720226,  0.359107733,
    0.360496253,     0.361888379,   0.363287717,   0.364690065,  0.366095126,
    0.367503256,     0.368912846,   0.370322436,   0.371732682,  0.373146862,
    0.374568254,     0.375994653,   0.377423763,   0.378856659,  0.380291909,
    0.381727159,     0.383162796,   0.384605259,   0.386055112,  0.387511671,
    0.388970941,     0.3904351,     0.391902804,   0.393370807,  0.39483884,
    0.396311998,     0.397792131,   0.39927876,    0.40076825,   0.402264059,
    0.403763562,     0.405264348,   0.406765163,   0.408269763,  0.4097808,
    0.41129747,      0.412817746,   0.414345741,   0.415876031,  0.417408347,
    0.418940693,     0.420478493,   0.422027558,   0.42358157,   0.425139725,
    0.426706791,     0.428275585,   0.429847896,   0.431420237,  0.432995975,
    0.434584349,     0.43617788,    0.437777281,   0.439387828,  0.440999478,
    0.44261539,      0.444231272,   0.445848554,   0.447476834,  0.449109405,
    0.450749636,     0.452398032,   0.454047978,   0.455703348,  0.457359135,
    0.45901534,      0.46068567,    0.46235913,    0.464042515,  0.465738237,
    0.467438042,     0.469141811,   0.47084704,    0.472552776,  0.474278837,
    0.476008475,     0.477747768,   0.47949481,    0.481246531,  0.483002484,
    0.484761536,     0.486520559,   0.488295525,   0.49007535,   0.491865396,
    0.493663818,     0.495468467,   0.497276217,   0.499088615,  0.500900984,
    0.502730727,     0.50456804,    0.506413937,   0.508268774,  0.510135412,
    0.512003422,     0.513876319,   0.515749276,   0.517638385,  0.519537747,
    0.521445453,     0.52336818,    0.525301397,   0.527236819,  0.529181659,
    0.531126499,     0.533083379,   0.535052359,   0.537032485,  0.539030612,
    0.541041613,     0.543056309,   0.545081735,   0.547108054,  0.549148738,
    0.551209688,     0.553282499,   0.555370092,   0.557472944,  0.559582591,
    0.56169939,      0.563818514,   0.565950811,   0.568109334,  0.570278764,
    0.572460651,     0.574658453,   0.576865315,   0.579081297,  0.581301808,
    0.583531141,     0.585793793,   0.5880633,     0.5903458,    0.592649639,
    0.594967842,     0.597295165,   0.599629164,   0.601969361,  0.60434562,
    0.606734753,     0.609137475,   0.611560702,   0.614004552,  0.616457999,
    0.618920684,     0.621385455,   0.623897135,   0.62642771,   0.628975928,
    0.631544232,     0.6341362,     0.636741221,   0.639357984,  0.641974807,
    0.644638479,     0.647331953,   0.650043726,   0.652769446,  0.655526817,
    0.65830642,      0.661102474,   0.663898587,   0.666747272,  0.669639051,
    0.672550321,     0.675474823,   0.678430974,   0.681423664,  0.684435964,
    0.687448263,     0.690512419,   0.693642557,   0.696790159,  0.699954331,
    0.703134775,     0.706375957,   0.709642947,   0.712909937,  0.716222942,
    0.719636083,     0.723063469,   0.726511359,   0.729961574,  0.733494461,
    0.737070143,     0.740645885,   0.744265616,   0.748035312,  0.751828134,
    0.755651891,     0.759475648,   0.763373315,   0.767356575,  0.771339834,
    0.775363207,     0.779573262,   0.783815503,   0.788095951,  0.792376339,
    0.79671365,      0.801209986,   0.805706322,   0.810240984,  0.815023899,
    0.81986022,      0.824749053,   0.829637885,   0.8346017,    0.839726806,
    0.844911814,     0.850150049,   0.855706692,   0.861387193,  0.867147505,
    0.872928977,     0.878819525,   0.884788394,   0.890981376,  0.897284448,
    0.903945148,     0.91087532,    0.917930007,   0.925089538,  0.93234235,
    0.939603746,     0.947228312,   0.955229938,   0.962519467,  0.969258308,
    0.976183772,     0.984259069,   0.993838668,   1.00341821,   1.01299775,
    1.02326667,      1.03450596,    1.04615545,    1.06060302,   1.07758701,
    1.09464693,      1.11170697,    1.12896502,    1.15077579,   1.17671537,
    1.21227145,      1.33260703,    1.47303271,    1.6134584,    1.79885411,
    2.92155123,      4.17932606,    5.43710089,    6.69487572,   7.95265055,
    9.21042538,      10.4682007,    11.7258329};