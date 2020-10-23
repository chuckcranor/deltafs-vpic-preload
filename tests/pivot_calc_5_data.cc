const int num_ranks = 512;

const float oob_data[] = {};

const int oob_data_sz = 0;

const float range_min = -0.000036;

const float range_max = 12.118516;

const int num_pivots = 64;

const float rank_bin_counts[] = {
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 144032.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0,      0.0, 0.0, 0.0};

const float rank_bins[] = {
    -3.6e-05,  0.0059,   0.011836, 0.017771, 0.023706, 0.029641, 0.034613,
    0.039295,  0.043255, 0.04695,  0.050376, 0.05332,  0.056263, 0.059401,
    0.063305,  0.067193, 0.070975, 0.074757, 0.078539, 0.082288, 0.086032,
    0.089775,  0.093518, 0.097246, 0.100435, 0.103586, 0.106674, 0.109666,
    0.112688,  0.115545, 0.118402, 0.121254, 0.124069, 0.126697, 0.129264,
    0.13172,   0.134175, 0.136402, 0.138619, 0.140633, 0.142525, 0.144374,
    0.146225,  0.148076, 0.150016, 0.152527, 0.155383, 0.15824,  0.161096,
    0.163953,  0.166893, 0.169196, 0.171107, 0.17364,  0.176384, 0.179116,
    0.181838,  0.184326, 0.186716, 0.189103, 0.191925, 0.194762, 0.197603,
    0.200444,  0.203219, 0.205704, 0.208172, 0.210592, 0.213012, 0.215511,
    0.218102,  0.220692, 0.223076, 0.225408, 0.227693, 0.229564, 0.231449,
    0.233194,  0.234663, 0.236813, 0.23883,  0.240848, 0.243356, 0.246406,
    0.24924,   0.252012, 0.254784, 0.257556, 0.260222, 0.262617, 0.26483,
    0.267044,  0.269486, 0.27219,  0.275142, 0.278179, 0.28121,  0.284242,
    0.28701,   0.289633, 0.292274, 0.294731, 0.296547, 0.297859, 0.300092,
    0.302572,  0.305236, 0.307909, 0.310583, 0.313029, 0.315474, 0.318011,
    0.320492,  0.322974, 0.325589, 0.3282,   0.330811, 0.333358, 0.335811,
    0.338275,  0.340739, 0.343245, 0.345793, 0.34828,  0.350682, 0.352685,
    0.354525,  0.356475, 0.358426, 0.360377, 0.362362, 0.364458, 0.366635,
    0.368812,  0.371072, 0.374082, 0.377092, 0.380127, 0.383257, 0.386402,
    0.389547,  0.392582, 0.395603, 0.398859, 0.402186, 0.405428, 0.408073,
    0.410782,  0.413546, 0.416118, 0.418626, 0.42147,  0.424749, 0.428055,
    0.431151,  0.43399,  0.436829, 0.439668, 0.442553, 0.445755, 0.449067,
    0.452209,  0.455351, 0.458792, 0.462302, 0.465768, 0.468304, 0.470458,
    0.473916,  0.477491, 0.480971, 0.484623, 0.488182, 0.491731, 0.495232,
    0.49873,   0.502267, 0.505629, 0.508828, 0.512027, 0.515388, 0.517981,
    0.520706,  0.523439, 0.527149, 0.531067, 0.5334,   0.536408, 0.54044,
    0.544413,  0.548105, 0.551148, 0.554191, 0.557653, 0.56164,  0.565617,
    0.569384,  0.573214, 0.577333, 0.58134,  0.585375, 0.590086, 0.59489,
    0.599772,  0.604623, 0.609113, 0.613392, 0.617618, 0.621422, 0.625227,
    0.628458,  0.63126,  0.633601, 0.636727, 0.640319, 0.64397,  0.648415,
    0.653729,  0.659066, 0.664625, 0.670287, 0.676132, 0.682036, 0.687941,
    0.693686,  0.699153, 0.704498, 0.709842, 0.715204, 0.721394, 0.727927,
    0.734247,  0.740336, 0.745961, 0.751164, 0.756297, 0.761224, 0.76648,
    0.772311,  0.778141, 0.783971, 0.790152, 0.797356, 0.804589, 0.811821,
    0.818392,  0.824745, 0.831039, 0.837658, 0.844256, 0.850919, 0.857583,
    0.86425,   0.871714, 0.879483, 0.887849, 0.896033, 0.904046, 0.912264,
    0.920247,  0.92756,  0.934283, 0.940111, 0.946103, 0.95109,  0.954568,
    0.958046,  0.962447, 0.967099, 0.971178, 0.974776, 0.977797, 0.980706,
    0.98423,   0.990759, 0.999255, 1.007752, 1.016146, 1.024121, 1.03177,
    1.036527,  1.043381, 1.050907, 1.057863, 1.065034, 1.073526, 1.084031,
    1.091292,  1.096254, 1.103034, 1.11157,  1.11908,  1.127144, 1.134604,
    1.141028,  1.149166, 1.157004, 1.164842, 1.17268,  1.178729, 1.183918,
    1.188995,  1.195072, 1.202741, 1.213776, 1.22543,  1.237084, 1.251167,
    1.265323,  1.278386, 1.290624, 1.298224, 1.305595, 1.318969, 1.33239,
    1.346344,  1.360185, 1.373126, 1.386217, 1.399738, 1.416524, 1.432438,
    1.445821,  1.459185, 1.474304, 1.488342, 1.501722, 1.514912, 1.528266,
    1.541435,  1.554129, 1.567928, 1.58232,  1.594807, 1.605997, 1.615947,
    1.62592,   1.641177, 1.654745, 1.668491, 1.676721, 1.68565,  1.698827,
    1.712003,  1.725518, 1.738341, 1.750492, 1.763352, 1.775151, 1.787303,
    1.799294,  1.807736, 1.815676, 1.826379, 1.837292, 1.848708, 1.860125,
    1.868947,  1.876727, 1.882494, 1.890587, 1.89963,  1.907714, 1.916419,
    1.927046,  1.937795, 1.950186, 1.962566, 1.974416, 1.986142, 1.996834,
    2.007622,  2.019384, 2.033257, 2.047638, 2.06202,  2.074035, 2.080549,
    2.085325,  2.093,    2.103099, 2.114243, 2.126034, 2.137022, 2.148421,
    2.16014,   2.172745, 2.185399, 2.197996, 2.2087,   2.21471,  2.221214,
    2.231972,  2.243325, 2.255412, 2.268201, 2.280556, 2.289288, 2.296617,
    2.305317,  2.317917, 2.330565, 2.343187, 2.357429, 2.372247, 2.38381,
    2.391875,  2.410805, 2.430067, 2.449329, 2.463929, 2.477883, 2.495253,
    2.51303,   2.524785, 2.53806,  2.548386, 2.556046, 2.564153, 2.57691,
    2.589037,  2.600113, 2.610596, 2.619528, 2.628834, 2.639236, 2.651401,
    2.667201,  2.684588, 2.702134, 2.719679, 2.741195, 2.754799, 2.765487,
    2.777243,  2.789791, 2.800143, 2.807946, 2.816113, 2.829786, 2.849999,
    2.870271,  2.890492, 2.909594, 2.923058, 2.93868,  2.948313, 2.960868,
    2.980654,  3.002173, 3.025869, 3.047728, 3.068252, 3.088593, 3.10342,
    3.114756,  3.137598, 3.164044, 3.179924, 3.18941,  3.203241, 3.219914,
    3.240831,  3.265248, 3.294189, 3.311882, 3.325635, 3.341786, 3.357372,
    3.375646,  3.403979, 3.430565, 3.440535, 3.450455, 3.470401, 3.493802,
    3.526133,  3.569954, 3.608585, 3.638639, 3.671268, 3.696192, 3.734678,
    3.777428,  3.840322, 3.914295, 3.981195, 4.023664, 4.062622, 4.097191,
    4.144344,  4.212564, 4.296432, 4.367468, 4.466233, 4.56021,  4.621121,
    4.701968,  4.773324, 4.842683, 5.027715, 5.285512, 5.572999, 5.920287,
    6.311256,  6.817053, 7.311542, 8.066168, 8.876624, 9.68708,  10.497536,
    11.307992, 12.118516};