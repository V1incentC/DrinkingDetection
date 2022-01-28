#include "calculation_features.h"
#include "arm_math.h"
#include "float.h"

//you should only change the CF_BUFFER_SIZE

#define  CF_FS                52 //sampling frequency of the signal
#define  CF_M                 256 //window length
#define  CF_D                 128 //overlap length
#define  CF_BUFFER_SIZE       256 //length of the array
#define  CF_L                 (CF_BUFFER_SIZE - CF_M)/ CF_D + 1//number of windows in the signal
#define CF_SAMPLING_FREQENCY  52
#define CF_TIMESTEP           1/CF_SAMPLING_FREQENCY


const float32_t window[] = {      0.0  , 0.00015059, 0.00060227, 0.00135477, 0.00240764, 0.00376023,
                         0.00541175, 0.00736118, 0.00960736, 0.01214894, 0.01498437, 0.01811197,
                         0.02152983, 0.02523591, 0.02922797, 0.0335036 , 0.03806023, 0.04289512,
                         0.04800535, 0.05338785, 0.05903937, 0.0649565 , 0.07113569, 0.07757322,
                         0.08426519, 0.09120759, 0.09839623, 0.10582679, 0.11349478, 0.12139558,
                         0.12952444, 0.13787647, 0.14644662, 0.15522973, 0.16422053, 0.17341357,
                         0.18280336, 0.1923842 , 0.20215034, 0.2120959 , 0.22221488, 0.2325012 ,
                         0.24294862, 0.25355092, 0.26430163, 0.27519435, 0.28622246, 0.29737934,
                         0.30865827, 0.32005247, 0.33155507, 0.34315914, 0.35485765, 0.3666436 ,
                         0.3785099 , 0.39044937, 0.40245485, 0.41451904, 0.42663476, 0.43879467,
                         0.45099142, 0.4632177 , 0.47546616, 0.4877294 , 0.5       , 0.5122706 ,
                         0.5245338 , 0.53678226, 0.54900855, 0.5612053 , 0.5733652 , 0.5854809 ,
                         0.59754515, 0.6095506 , 0.62149006, 0.6333564 , 0.6451423 , 0.65684086,
                         0.66844493, 0.6799475 , 0.6913417 , 0.7026207 , 0.71377754, 0.72480565,
                         0.73569834, 0.7464491 , 0.75705135, 0.7674988 , 0.7777851 , 0.7879041 ,
                         0.79784966, 0.8076158 , 0.81719667, 0.8265864 , 0.8357795 , 0.84477025,
                         0.8535534 , 0.86212355, 0.8704756 , 0.8786044 , 0.88650525, 0.8941732 ,
                         0.90160376, 0.90879244, 0.9157348 , 0.92242676, 0.9288643 , 0.9350435 ,
                         0.94096065, 0.9466122 , 0.95199466, 0.95710486, 0.96193975, 0.9664964 ,
                         0.970772  , 0.9747641 , 0.97847015, 0.98188806, 0.98501563, 0.9878511 ,
                         0.9903926 , 0.9926388 , 0.99458826, 0.9962398 , 0.9975924 , 0.99864525,
                         0.99939775, 0.99984944, 1.0        , 0.99984944, 0.99939775, 0.99864525,
                         0.9975924 , 0.9962398 , 0.99458826, 0.9926388 , 0.9903926 , 0.9878511 ,
                         0.98501563, 0.98188806, 0.97847015, 0.9747641 , 0.970772  , 0.9664964 ,
                         0.96193975, 0.95710486, 0.95199466, 0.9466122 , 0.94096065, 0.9350435 ,
                         0.9288643 , 0.92242676, 0.9157348 , 0.90879244, 0.90160376, 0.8941732 ,
                         0.88650525, 0.8786044 , 0.8704756 , 0.86212355, 0.8535534 , 0.84477025,
                         0.8357795 , 0.8265864 , 0.81719667, 0.8076158 , 0.79784966, 0.7879041 ,
                         0.7777851 , 0.7674988 , 0.75705135, 0.7464491 , 0.73569834, 0.72480565,
                         0.71377754, 0.7026207 , 0.6913417 , 0.6799475 , 0.66844493, 0.65684086,
                         0.6451423 , 0.6333564 , 0.62149006, 0.6095506 , 0.59754515, 0.5854809 ,
                         0.5733652 , 0.5612053 , 0.54900855, 0.53678226, 0.5245338 , 0.5122706 ,
                         0.5       , 0.4877294 , 0.47546616, 0.4632177 , 0.45099142, 0.43879467,
                         0.42663476, 0.41451904, 0.40245485, 0.39044937, 0.3785099 , 0.3666436 ,
                         0.35485765, 0.34315914, 0.33155507, 0.32005247, 0.30865827, 0.29737934,
                         0.28622246, 0.27519435, 0.26430163, 0.25355092, 0.24294862, 0.2325012 ,
                         0.22221488, 0.2120959 , 0.20215034, 0.1923842 , 0.18280336, 0.17341357,
                         0.16422053, 0.15522973, 0.14644662, 0.13787647, 0.12952444, 0.12139558,
                         0.11349478, 0.10582679, 0.09839623, 0.09120759, 0.08426519, 0.07757322,
                         0.07113569, 0.0649565 , 0.05903937, 0.05338785, 0.04800535, 0.04289512,
                         0.03806023, 0.0335036 , 0.02922797, 0.02523591, 0.02152983, 0.01811197,
                         0.01498437, 0.01214894, 0.00960736, 0.00736118, 0.00541175, 0.00376023,
                         0.00240764, 0.00135477, 0.00060227, 0.00015059};

    const float32_t std_vals[294] = {4.00317337e-01, 1.06465377e-01, 3.62314233e+01, 2.77718093e-01,
                                     5.69232358e+00, 4.38478278e-01, 1.02481238e+02, 6.96354205e+01,
                                     1.36494606e+01, 1.75022977e+01, 1.74574763e-01, 3.76975072e+00,
                                     9.48668800e-01, 3.37188293e-01, 1.08326419e-01, 3.23870145e+01,
                                     2.97332453e-01, 4.60004972e+00, 4.82458783e-01, 8.63202028e+01,
                                     7.48415317e+01, 2.32545632e+01, 1.68171043e+00, 1.77113980e-01,
                                     4.57722152e+00, 9.90164248e-01, 4.58080585e-01, 1.55054088e-01,
                                     7.50899144e+01, 3.27064770e-01, 4.74280026e+00, 5.86257523e-01,
                                     1.17268629e+02, 7.72620472e+01, 1.29861895e+01, 9.06169413e+01,
                                     2.69999858e-01, 4.21414343e+00, 9.85650117e-01, 2.33638302e+01,
                                     2.89921468e+01, 7.45286707e+02, 2.28389350e+01, 3.53375529e+01,
                                     1.76365339e+02, 5.98114052e+03, 7.52599984e+03, 3.71420525e+01,
                                     6.79664488e+02, 3.07153722e+01, 6.55480452e+00, 1.43871396e+00,
                                     3.38269894e+01, 1.83191171e+01, 1.52157228e+03, 3.36392257e+01,
                                     3.70780184e+01, 9.26737365e+01, 8.65970929e+03, 8.93341644e+03,
                                     3.79604471e+01, 1.83779217e+03, 2.56528193e+01, 5.32387540e+00,
                                     1.21981828e+00, 3.15757878e+01, 2.41458354e+01, 2.86331818e+03,
                                     3.13368007e+01, 3.56766763e+01, 1.11568706e+02, 8.08340168e+03,
                                     9.03601137e+03, 3.68744426e+01, 8.53900357e+02, 3.41737103e+01,
                                     5.25867821e+00, 1.32055331e+00, 2.13111016e+00, 2.22591998e+00,
                                     2.31038558e+00, 2.42212108e+00, 2.44287960e+00, 2.46991784e+00,
                                     2.50996165e+00, 2.52307282e+00, 2.56844600e+00, 2.58536443e+00,
                                     1.00403391e-01, 5.19703804e-02, 2.35627836e-02, 1.44722659e-02,
                                     9.63905953e-03, 7.25143425e-03, 5.76532819e-03, 4.69449933e-03,
                                     3.69618746e-03, 3.01214304e-03, 7.15789056e+01, 1.47317037e+00,
                                     3.86493911e-01, 2.06049194e-01, 1.40778218e-01, 9.53693313e-02,
                                     7.81550428e-02, 6.99304039e-02, 5.72411889e-02, 5.23173917e-02,
                                     4.61718386e-02, 8.70011579e-02, 2.72013357e-01, 2.39849830e+00,
                                     3.12841161e+01, 1.74023140e-01, 2.51386494e+00, 2.64351511e+00,
                                     2.71918304e+00, 2.74013407e+00, 2.74849477e+00, 2.76711104e+00,
                                     2.74789116e+00, 2.76827056e+00, 2.76835800e+00, 2.79003062e+00,
                                     8.45234462e-02, 4.48376288e-02, 2.14752116e-02, 1.32607845e-02,
                                     9.46803316e-03, 7.22141921e-03, 5.67809957e-03, 4.62412121e-03,
                                     3.79573380e-03, 3.16239130e-03, 7.64420986e+01, 1.38337639e+00,
                                     3.90568651e-01, 1.73485002e-01, 1.07549569e-01, 1.01861099e-01,
                                     9.92275852e-02, 8.85482265e-02, 8.33030337e-02, 7.53737516e-02,
                                     6.59491565e-02, 1.11459380e-01, 2.92349732e-01, 2.41743864e+00,
                                     3.07885852e+01, 1.76607798e-01, 2.62742867e+00, 2.72708247e+00,
                                     2.69682925e+00, 2.68328334e+00, 2.70202251e+00, 2.70307415e+00,
                                     2.73810413e+00, 2.72322851e+00, 2.73474976e+00, 2.73854517e+00,
                                     1.80914765e-01, 9.34735023e-02, 4.04234675e-02, 1.96264114e-02,
                                     1.17326532e-02, 7.87812308e-03, 5.77425604e-03, 4.45901548e-03,
                                     3.54880672e-03, 2.93986430e-03, 8.74776633e+01, 1.61758918e+00,
                                     3.89573702e-01, 1.51661221e-01, 1.10777768e-01, 9.94876972e-02,
                                     8.46100887e-02, 8.21702274e-02, 7.30032005e-02, 6.97616865e-02,
                                     5.97959676e-02, 1.13399656e-01, 3.01804879e-01, 2.50484723e+00,
                                     3.24531791e+01, 2.68851617e-01, 5.00915478e+00, 5.01345902e+00,
                                     4.95087061e+00, 4.84899737e+00, 4.85219893e+00, 4.83486155e+00,
                                     4.74998286e+00, 4.72902667e+00, 4.66752741e+00, 4.63275716e+00,
                                     5.31802106e+03, 3.44375693e+03, 2.25742906e+03, 1.71879014e+03,
                                     1.18043894e+03, 9.58596194e+02, 7.81032753e+02, 6.46380377e+02,
                                     5.37629526e+02, 4.49113973e+02, 3.19523849e+06, 3.55365165e-01,
                                     3.65928012e-01, 2.20738634e-01, 1.28235615e-01, 9.42314958e-02,
                                     8.45699547e-02, 7.35166264e-02, 6.06651019e-02, 5.69267084e-02,
                                     5.27582690e-02, 2.43404141e-01, 2.93984375e+01, 1.89040977e+00,
                                     2.14893812e+01, 3.05792115e+01, 4.83662922e+00, 4.77774996e+00,
                                     4.72638210e+00, 4.71921127e+00, 4.64354225e+00, 4.61933630e+00,
                                     4.62941851e+00, 4.56104041e+00, 4.55810489e+00, 4.55480862e+00,
                                     5.67661274e+03, 2.84241836e+03, 1.38569426e+03, 8.36565908e+02,
                                     6.14614269e+02, 4.96767769e+02, 4.13381034e+02, 3.59527706e+02,
                                     2.96424387e+02, 2.45026698e+02, 4.04419177e+06, 4.22769746e-01,
                                     3.65197723e-01, 2.53461918e-01, 1.52285977e-01, 1.10208748e-01,
                                     7.83605652e-02, 5.60803484e-02, 3.87207189e-02, 3.79580630e-02,
                                     4.53499075e-02, 2.40029782e-01, 3.48961573e+01, 1.86634305e+00,
                                     2.18001204e+01, 2.55487687e+01, 4.63292036e+00, 4.64251758e+00,
                                     4.51871251e+00, 4.54362103e+00, 4.48616207e+00, 4.45440554e+00,
                                     4.44487241e+00, 4.43705671e+00, 4.35006101e+00, 4.33846578e+00,
                                     9.92660524e+03, 6.10725371e+03, 2.68097156e+03, 1.53662711e+03,
                                     9.89309020e+02, 6.56820127e+02, 5.14781325e+02, 4.02205351e+02,
                                     3.15913590e+02, 2.67713006e+02, 3.87487201e+06, 4.64110731e-01,
                                     3.66600039e-01, 2.41089161e-01, 1.41369989e-01, 1.02936309e-01,
                                     8.10276704e-02, 5.26151817e-02, 4.26778668e-02, 4.12117946e-02,
                                     7.10179437e-02, 2.24869881e-01, 3.52969195e+01, 1.87907095e+00,
                                     2.17468413e+01, 3.39785556e+01};    

  const float32_t mean_vals[294] = { -1.11310375e-01,  8.61328398e-02, -4.83744315e-01,  3.09057648e-01,
                                      2.53850763e+00,  3.71312290e-01, -2.84954559e+01,  8.26261398e+01,
                                      6.94001997e+00,  5.16407310e+00,  1.20463198e-01,  1.01777589e+00,
                                     -1.17858293e-01,  5.24429099e-01,  9.96401674e-02,  3.41361984e-01,
                                      5.48010253e-01,  1.30249679e+00,  4.64719097e-01,  1.34253850e+02,
                                      1.41800032e+02,  2.53815666e+01,  5.42600722e+00,  1.36381894e-01,
                                      1.52896537e+00, -1.15871171e-01, -4.19144179e-01,  1.34360065e-01,
                                      2.50752421e-01,  5.27776753e-01,  2.09242403e+00,  5.56445967e-01,
                                     -1.07300910e+02,  1.43547455e+02,  5.03623912e+00,  4.78252121e+00,
                                      1.93723028e-01,  1.23972165e+00,  5.49405442e-02,  3.89985506e-01,
                                      2.53907501e+01, -8.11317395e-03,  4.94000982e+00,  4.43557426e+01,
                                      1.55502451e+02,  9.98362897e+01,  4.64607223e+03,  3.94662006e+01,
                                     -1.00744663e+01,  2.17634232e+01,  4.15433510e+00,  1.62640059e-01,
                                      3.15678243e+00,  1.29935741e+01,  1.26938517e+01,  4.75741349e+00,
                                      3.85603367e+01,  7.06820323e+01,  8.08136301e+02,  2.94854736e+03,
                                      3.54784135e+01, -1.51707457e+01,  1.30367486e+01,  2.89708486e+00,
                                     -7.25924954e-02, -2.67507545e+00,  1.44299764e+01,  1.28787711e+01,
                                      4.71076734e+00,  3.75926951e+01,  7.52679529e+01, -6.84819316e+02,
                                      3.10034332e+03,  3.37685547e+01, -1.43530757e+01,  1.40805239e+01,
                                      3.04639601e+00,  4.99448277e-02,  1.16344276e+00,  1.35839858e+00,
                                      1.68839840e+00,  2.04898947e+00,  2.33085921e+00,  2.57657565e+00,
                                      2.77976865e+00,  2.97008199e+00,  3.14192578e+00,  3.29973070e+00,
                                      3.43381859e-02,  1.76804407e-02,  8.08197155e-03,  4.67567901e-03,
                                      3.08338066e-03,  2.27207785e-03,  1.76344184e-03,  1.40983784e-03,
                                      1.12675003e-03,  9.21937286e-04,  4.89978131e+01,  2.89402645e+00,
                                      6.62207576e-01,  1.11067176e-01,  6.30873572e-02,  3.72597104e-02,
                                      2.82709026e-02,  2.33588226e-02,  1.78721724e-02,  1.53887226e-02,
                                      1.24789458e-02,  2.90086146e-02,  3.22758357e-01,  6.43267598e+00,
                                      5.01580154e+01,  1.20015350e-01,  1.48643708e+00,  1.75442400e+00,
                                      2.17282021e+00,  2.48009256e+00,  2.73245225e+00,  2.97157561e+00,
                                      3.18806066e+00,  3.36455807e+00,  3.56337611e+00,  3.75455330e+00,
                                      3.64324176e-02,  1.94314450e-02,  9.54031240e-03,  5.75779449e-03,
                                      3.93819629e-03,  2.88080018e-03,  2.19978577e-03,  1.74686270e-03,
                                      1.41818062e-03,  1.17232253e-03,  1.05058457e+02,  2.57874881e+00,
                                      6.21734014e-01,  8.97941739e-02,  4.75941272e-02,  4.35098669e-02,
                                      4.09463004e-02,  3.57476249e-02,  3.15128585e-02,  2.69157723e-02,
                                      2.03955149e-02,  4.18497473e-02,  5.53906375e-01,  6.08900478e+00,
                                      4.60718934e+01,  1.35943800e-01,  1.40406531e+00,  1.64531205e+00,
                                      1.97809290e+00,  2.26961541e+00,  2.54477502e+00,  2.75850692e+00,
                                      3.01434985e+00,  3.19793079e+00,  3.42162755e+00,  3.56958464e+00,
                                      8.19672595e-02,  4.07575933e-02,  1.78817442e-02,  9.12611273e-03,
                                      5.42558637e-03,  3.58607192e-03,  2.55037863e-03,  1.92361104e-03,
                                      1.50472037e-03,  1.21444713e-03,  1.09469202e+02,  2.55870188e+00,
                                      6.72169716e-01,  7.09594970e-02,  4.50880655e-02,  3.92183843e-02,
                                      3.26983089e-02,  3.05981754e-02,  2.61255415e-02,  2.35689123e-02,
                                      1.80777910e-02,  4.14956079e-02,  5.60732251e-01,  6.44881910e+00,
                                      5.08895572e+01,  1.92741591e-01,  2.67249028e+00,  2.79157267e+00,
                                      2.97789561e+00,  3.11293961e+00,  3.28638848e+00,  3.45679439e+00,
                                      3.59566718e+00,  3.77150137e+00,  3.94006813e+00,  4.08604116e+00,
                                      1.82954495e+03,  1.29457263e+03,  8.76897090e+02,  6.47880590e+02,
                                      4.92061005e+02,  3.82808366e+02,  3.03356694e+02,  2.43178719e+02,
                                      1.96926666e+02,  1.60877295e+02,  5.20001398e+05,  4.77565221e+00,
                                      4.93287263e-01,  1.93236640e-01,  7.05682210e-02,  4.28908986e-02,
                                      3.16099485e-02,  2.54271547e-02,  1.92625268e-02,  1.65596335e-02,
                                      1.29855659e-02,  9.41721478e-02,  1.81487197e+01,  4.92159848e+00,
                                      3.01091668e+01,  2.16413075e+01,  2.48800649e+00,  2.55767139e+00,
                                      2.71723744e+00,  2.87573901e+00,  2.97990753e+00,  3.12654935e+00,
                                      3.27374848e+00,  3.39489852e+00,  3.53512560e+00,  3.68480480e+00,
                                      7.59674308e+02,  4.66956333e+02,  2.75874801e+02,  1.86902310e+02,
                                      1.37635020e+02,  1.04885311e+02,  8.19204278e+01,  6.51566533e+01,
                                      5.21076451e+01,  4.22369906e+01,  4.24615286e+05,  4.64230858e+00,
                                      4.81097596e-01,  2.35254606e-01,  8.79570453e-02,  4.61676425e-02,
                                      2.57983470e-02,  1.50875283e-02,  9.34796022e-03,  8.13600468e-03,
                                      8.08834602e-03,  8.30649246e-02,  1.15177631e+01,  5.11991201e+00,
                                      3.19268693e+01,  1.29665544e+01,  2.33636552e+00,  2.44223944e+00,
                                      2.59155639e+00,  2.74520371e+00,  2.87641336e+00,  3.00959035e+00,
                                      3.14362003e+00,  3.30084534e+00,  3.40357598e+00,  3.56277420e+00,
                                      1.46852067e+03,  9.09722092e+02,  4.46476991e+02,  2.84092835e+02,
                                      1.94697354e+02,  1.35091009e+02,  9.92987279e+01,  7.49152086e+01,
                                      5.68142242e+01,  4.41642447e+01,  4.59630587e+05,  4.59025275e+00,
                                      5.17321345e-01,  2.15083351e-01,  7.64199752e-02,  4.13253445e-02,
                                      2.57176093e-02,  1.49153730e-02,  1.02297770e-02,  9.12541082e-03,
                                      1.18121085e-02,  7.80497053e-02,  1.21107161e+01,  5.17734270e+00,
                                      3.25800884e+01,  1.40027900e+01};                                                  
//calculates mean value
//takes in array and its size
float CF_mean_value(float *arr, int n)
{
    float sum = 0;
    for (int i = 0; i < n; i++)
        sum = sum + arr[i];
    return sum / n;
}

//calculate standard deviation
//takes in array and its size
float CF_standard_deviation(float *arr, int n)
{
    float sum = 0;
    float meanVal = CF_mean_value(arr,n);

    for(int i = 0; i < n; i++)
        sum = sum + (arr[i]-meanVal)*
                    (arr[i]-meanVal);
    return sqrt(sum/(n));

}

//takes in arrray and its size
//function calls standard_deviation and mean_value
float CF_coefficient_of_variation(float *arr, int n)
{
    return CF_standard_deviation(arr,n)/CF_mean_value(arr,n);
}

/*number of zero crossings*/
//if you multiply two adjacent values and have a negative result it means you have crossed the 0 line
int CF_num_of_zero_crossings(float *arr, int n)
{
    int count = 0;
    for(int i = 1; i < n; i++)
    {
        if(arr[i]*arr[i-1] < 0)
            count++;
    }
    return count;
}

/* amplitude of a signal */
//finds max and min value and subtracts them
float CF_calc_amplitude(float *arr, int n)
{
    float max = arr[0];
    float min = arr[0];

    for(int i = 0; i < n; i++)
    {
        if(arr[i] > max)
            max = arr[i];
        if(arr[i] < min)
            min = arr[i];
    }
    return max - min;
}


/* velocity*/
//rewritten in c-code from notes
float CF_comput_AC_velocity(float *arr, int n, float timestep)
{
    float calc = 0;
    float dT = 1.0;

    for(int i = 0; i < n; i++)
    {
        calc += arr[i]*dT;
        dT = timestep;
    }
    return calc;
}
/*calculate sum per component*/
//rewritten in c-code from notes
float CF_calc_sum_per_component(float *arr, int n, float timestep)
{
    float calc = 0;
    float dT = 1;

    for(int i = 0; i < n; i++)
    {
        calc += fabs(arr[i])*dT;
        if(i < n-1)
            dT = timestep; //this never happens??
    }
    return calc;
}

int CF_calc_mean_crossing_rate(float *arr, int n)
{
    float mean = CF_abs_mean_value(arr,n); //using abs mean because in kotlin there was a math.abs function used

    int crossings = 0;
    float last = arr[0] - mean;
    float current = 0;

    for(int i = 1; i < n; i++)
    {
        current = arr[i]-mean;
        if(last*current < 0) crossings++;
        last = current;
    }
    return crossings;
}
//support function for qsort()
int CF_cmpfunc (const void * a, const void * b) {
    return (*(float*)a >= *(float*)b) ? 1 : -1;
   //return (  *(int*)a - *(int*)b);
}
// Function to give index of the median 
int32_t median(float32_t* a, int32_t l, int32_t r) 
{ 
    int32_t n = r - l + 1; 
    n = (n + 1) / 2 - 1; 
    return n + l; 
} 
  
// Function to calculate IQR 
float32_t CF_IQR(float32_t* a, uint32_t n) 
{ 
    float32_t temp_arr[n];
    //store array in temp_array to preserve it
    for(uint32_t i = 0; i < n; *(temp_arr + i++) = *(a + i));
    //sort the temp_arr
    qsort(temp_arr,n,sizeof(float32_t), CF_cmpfunc);  
  
    // Index of median of entire data 
    int32_t mid_index = median(temp_arr, 0, n); 
    int32_t second_index = 0;
    // Median of first half 
    float32_t Q1 = temp_arr[median(temp_arr, 0, mid_index)]; 
  
    // Median of second half 
    second_index = /* mid_index + */ median(temp_arr, mid_index + 1, n); //preserving original code that worked weird
    float32_t Q3 = temp_arr[second_index]; 
  
    // IQR calculation 
    return (Q3 - Q1); 
} 

float32_t CF_calc_entropy(float32_t* arr, uint32_t num_symbols)
{// dodadi normalizacija na nizata p = 1.0*p/suma od p i na kraj bez logaritmot i OK ke e
  int32_t i;
  float32_t entropy = 0,sum = 0;
  for(i = 0; i < num_symbols;i++)
  {
    sum += *(arr+i);
  }

  float32_t temp = 0;
  
   for(i=0; i< num_symbols; i++)
   {     
         temp = arr[i] / sum;

	 if(temp > 0.0)
        { entropy -= temp * (float32_t) log( (float32_t) temp);}
   }

   //entropy = entropy/ (float) log ((float) 2.0);
   return entropy;

}
void CF_absolute_value_complex(float32_t* niza, uint32_t n, float32_t* energy_feat, float32_t* entropy)
{
       
	float32_t absolute_array[n/2]; //array half the size of input array to store calculated absolute values
	float32_t sum   = 0.0;// In this variable we will have sum of square of absolute values
	int32_t counter = 0;
	for(uint32_t i=0;i<n;i+=2)
	{	
        float re,img,abs_value;
		re = niza[i];
		img = niza[i+1];
		abs_value = sqrt(re*re + img*img);
		
        absolute_array[counter] = abs_value;
        counter++;

		abs_value = abs_value*abs_value;
		sum += abs_value;
	}
	
	*energy_feat = sum/n*2;
	*entropy = 0.0;
	*entropy = CF_calc_entropy(absolute_array,n/2);
	
}


/*absolute mean value */
//just calculate mean value and take the absolute of it
float32_t CF_abs_mean_value(float32_t* arr, uint32_t n)
{
    float32_t sum  = 0;
    float32_t temp = 0;
    for (int32_t i = 0; i < n; i++)
    {
        temp = *(arr + i);  
        sum += fabs(temp);
    }
    return (float) sum / (float) n;
}

//fills and array with sample frequenices
//input is array, it's size, lenght of fft, sample frequency
void CF_sample_frequencies(float32_t* niza, uint32_t n,int32_t nfft, int32_t fs)
{

	int32_t i;
	for(i=0;i<n;i++) 
	{
		niza[i] = (float) (i*fs)/nfft;
	}

}
//subtract the mean value of array at every element
void CF_detrend_func(float32_t* niza, uint32_t n)
{
	float32_t sum  = 0.0;
	float32_t mean = 0.0;
	int32_t i      = 0;

	for (i = 0; i < n; i++) 
        {
          sum += *(niza+i);
        }

	mean = sum/n;
	
	for (i = 0; i < n; i++) 
        {
          *(niza+i) = *(niza+i) - mean;
        }
}
//multiply the input array with the window
void CF_window_data(float32_t* niza, uint32_t n, const float32_t *win_values)
{
	uint32_t i = 0;
	for(i=0;i<n;i++)
	{
		*(niza+i) = *(niza+i) * *(win_values+i);
	}
}

float32_t CF_func_skewness_and_kurtosis(float32_t* niza, uint32_t n, float32_t* skewness, float32_t* kurtosis) 
{
        float32_t mean  = 0.0;
        float32_t m2    = 0.0;
        float32_t m3    = 0.0;
	float32_t m4    = 0.0;
        uint32_t i;

        *skewness       = 0.0;
	*kurtosis       = 0.0;

        for(i=0;i<n;i++)
        {
                mean += *(niza+i);
        }
        mean = mean/n;
        //We calculated mean value, next we will calculate second and third moment around the mean
        for(i=0;i<n;i++)
        {
                m2 += (*(niza+i) - mean)*(*(niza+i) - mean);
                m3 += (*(niza+i) - mean)*(*(niza+i) - mean)*(*(niza+i)-mean);
		m4 += (*(niza+i) - mean)*(*(niza+i) - mean)*(*(niza+i)-mean)*(*(niza+i) - mean);
        }
        m2 = m2/ (float32_t) n;
        m3 = m3/ (float32_t) n;
	m4 = m4/ (float32_t) n;
        *skewness = m3/pow(m2,1.5);
        *kurtosis = m4/pow(m2,2);
	*kurtosis  = *kurtosis - 3;


} 
voidCF_conjugate_multipy(float32_t* niza, uint32_t n, float32_t* new_list, uint32_t m, float32_t scale)
{
	uint32_t i=0;
	float32_t a,b;
	uint32_t counter = 0;
	
        for(i=0;i<n;i++)
	{
          if(i==0)
          {
            new_list[counter] =niza[i]*niza[i];
            new_list[counter] *=scale;
            counter = counter +1;
          }
          else if(i==1)
          {
            new_list[m-1] =niza[i]*niza[i];
            new_list[m-1] *=scale;
          }
          else
          {
            if(i%2==0)
            {
              a = niza[i];
              b = niza[i+1];
              i = i+1;
              new_list[counter] =  a*a + b*b; //Hope that this is fine, have to check additionally	
              new_list[counter] *=scale;
              new_list[counter] *=2;
              counter = counter +1;
            }
	  }
	}
}
void CF_welch_method(float32_t* niza, uint32_t n, float32_t* output_arr, uint32_t out_length)
{
  //variables needed for arm_rfft
  uint32_t fftSize      = 256;
  uint32_t ifftFlag     = 0;
  uint32_t doBitReverse = 1;
  arm_rfft_fast_instance_f32 varInstRfftF32;
  arm_status status;
  status = ARM_MATH_SUCCESS;

  status = arm_rfft_fast_init_f32(&varInstRfftF32,fftSize);
  /*scale is dependent on the sampling frequency
    scale = 1/(sampling_freq * 96); 
    96 is dependent on the window type and length of npreseg. 
    If you change any of these two, recalculate it by (win*win).sum() 
    (win is an array which can be generated in python with the following function 
    https://docs.scipy.org/doc/scipy/reference/generated/scipy.signal.get_window.html)*/
  float32_t scale = (float) 1/(CF_FS * 96);;
  uint16_t l = (n-CF_M)/CF_D+1;

  CF_detrend_func(niza, n);
  //Next, we will divide the array in segments
  float32_t segment[CF_M];
  float32_t segment_fft_output[CF_M];
  float32_t final_arr[l][out_length];

  //setting the values of segment[] to 0
  for(uint16_t i = 0; i < CF_M; i++)
  {
    *(segment+i) = 0.0f;
    *(segment_fft_output+i) = 0.0f;
  }

  for(uint16_t i = 0; i<l; i++)
  {
      
      for(uint16_t j=0; j<CF_M; j++)
      {
        *(segment+j) = *(niza+(i*CF_D + j));
      }
      //After we have the selected segment, we need to detrend the values
      CF_detrend_func(segment,CF_M);
      CF_window_data(segment,CF_M,window);
      //preform the fft
      arm_rfft_fast_f32(&varInstRfftF32, segment, segment_fft_output, ifftFlag);
      //Now, in segment we have real discrete fourier transform of the windowed signal
      //Next, we need to multipy the values with their conjugte
      CF_conjugate_multipy(segment_fft_output,CF_M,final_arr[i],out_length,scale);
      
  }  
  //calculating the mean value of each column and storing them in the output_arr
  for(uint16_t i = 0; i < out_length;i++)
  {
    float32_t sum  = 0.0f;
    float32_t mean = 0.0f;
    for(uint16_t j = 0; j < l; j++)
    {
      sum += final_arr[j][i];
    }
    mean = (float) sum / l;
    *(output_arr+i) = mean;
  }
}


void CF_fft_and_flip_array(float32_t* input_arr, uint32_t n, float32_t* output_arr, uint32_t m)
{
  
  float32_t temp_arr_in[n] , temp_arr_out[n];
  //fill the temp arr with input arr values
  for(uint32_t i = 0; i < n; *(temp_arr_in + i++) = *(input_arr + i));
  //FFT
  //variables needed for arm_rfft
  uint32_t fftSize      = n;
  uint32_t ifftFlag     = 0;
  uint32_t doBitReverse = 1;
  arm_rfft_fast_instance_f32 varInstRfftF32;
  arm_status status;
  status = ARM_MATH_SUCCESS;

  status = arm_rfft_fast_init_f32(&varInstRfftF32,fftSize);
  arm_rfft_fast_f32(&varInstRfftF32, temp_arr_in, temp_arr_out, ifftFlag);

  float32_t temp;
  temp = *(temp_arr_out+1); // we need to save the second element
  *(temp_arr_out+1) = 0; //set second element to 0 because the DC component of fft can't have an imaginary part
  
  //moving the second element to the middle of output array and setting the +1 element to 0
  *(output_arr + n) = temp;
  *(output_arr + (n+1)) = 0.0f;

  for(uint32_t i = 0; i < m; i+=2)
  {
    //fil the output_arr with input values
    *(output_arr + i/2) = *(temp_arr_out + i/2);
    
    //flip the array values
    if(i < n)
    {
      //take the elements 
      *(output_arr + m-i) = *(temp_arr_out + i);
      *(output_arr + m-i+1) = *(temp_arr_out + i+1);
    }
  }

}
// summarize squared values of arr
float32_t CF_total_fft_sum(float32_t* arr, uint16_t n)
{
  float32_t sum  = 0;
  float32_t data = 0;
  for(uint16_t i = 0; i < n; i++)
  {
    sum += *(arr+i) * *(arr+i);
  }
  return sum;
}

float32_t normalized_squared_sum_N_elements(float32_t* arr, uint16_t n, uint16_t start, uint16_t stop, float32_t total_fft_sum)
{
  float32_t sum = 0;
  for(uint16_t i = start; i < stop;i++)
  {
    sum += *(arr+i) * *(arr+i);
  }
  return sum/total_fft_sum;
  
}
void CF_fill_bin_arr(float32_t* bin_arr, float32_t* input_arr, float32_t fft_sum)
{
  
  for(uint16_t i = 0; i < 45; i +=5)
  {
    
    *(bin_arr + i/5) = normalized_squared_sum_N_elements(input_arr, 129, i, i+5, fft_sum);

  }

  *(bin_arr + 9) = normalized_squared_sum_N_elements(input_arr, 129, 45, 129, fft_sum);
}

void CF_find_N_max_values(float32_t* input_arr, uint32_t arr_len, uint32_t n, float32_t* result_values, int16_t* result_indicies )
{
  float32_t temp_arr[arr_len];
  float32_t pResult = 0;
  uint32_t pIndex   = 0;
  //store array in temp_array to preserve it
  for(uint32_t i = 0; i < arr_len; *(temp_arr + i++) = *(input_arr + i));
  
  //finds the max value and puts it to -2147483648(smallest int value) so we can get the next biggest value
  for(uint32_t i = 0; i < n; i++)
  {
    arm_max_f32(temp_arr,arr_len,&pResult,&pIndex);
    *(result_values+i) = pResult;
    *(result_indicies + i) = pIndex;
    *(temp_arr + pIndex) = FLT_MIN;
  
  }
}
void CF_standardize_features(float *features)
{
  uint16_t len = sizeof(std_vals)/sizeof(std_vals[0]);
  for(uint16_t i = 0; i < len; i++)
  {
    features[i] = (features[i] - mean_vals[i])/std_vals[i];
  }
}

void CF_fill_raw_features(float32_t* features, float32_t* input_arr, uint16_t n)
{
  features[0] = CF_mean_value(input_arr, n);
  features[1] = CF_standard_deviation(input_arr,n);
  features[2] = CF_coefficient_of_variation(input_arr, n);
  features[3] = CF_abs_mean_value(input_arr, n);
  features[4] = CF_num_of_zero_crossings(input_arr, n);
  features[5] = CF_calc_amplitude(input_arr, n);
  features[6] = CF_comput_AC_velocity(input_arr, n, CF_TIMESTEP);
  features[7] = CF_calc_sum_per_component(input_arr, n, CF_TIMESTEP);
  features[8] = CF_calc_mean_crossing_rate(input_arr, n);
  features[9] = CF_calc_entropy(input_arr,n);
  features[10] = CF_IQR(input_arr, n);
  CF_func_skewness_and_kurtosis(input_arr,n,&features[12],&features[11]);
}

void CF_fill_frequency_features(float32_t* features, float32_t* input_arr, uint16_t n)
{
   float32_t bin_values[10], value_Pmax[10],sample_freq[129],Pxx_den[129],flipped_FFT[512];
   int16_t index_Pmax[10];
   float32_t energy = 0,entropy = 0,abs_mean = 0, skewness = 0, kurtosis = 0, iqr =0;

   abs_mean = CF_abs_mean_value(input_arr,n);
   iqr = CF_IQR(input_arr,n);

   CF_fft_and_flip_array(input_arr,n,flipped_FFT,2*n);
   CF_absolute_value_complex(flipped_FFT,2*n,&energy,&entropy);//calculates energy and entropy
   
   CF_welch_method(input_arr,n,Pxx_den,129); //changes the original array
   float fft_sum = CF_total_fft_sum(Pxx_den,129);
   CF_fill_bin_arr(bin_values,Pxx_den,fft_sum);
   CF_func_skewness_and_kurtosis(Pxx_den,129,&skewness,&kurtosis);
   CF_find_N_max_values(Pxx_den,129,10,value_Pmax,index_Pmax);
   CF_sample_frequencies(sample_freq,129,256,CF_SAMPLING_FREQENCY);

    for(int i = 0; i < 10; i++)
    {
        features[i] = sample_freq[index_Pmax[i]];//fill first 10 features
        features[10+i] = value_Pmax[i];
        features[22+i] = bin_values[i];
    }
    features[20] = energy;
    features[21] = entropy;
    features[32] = abs_mean;
    features[33] = skewness;
    features[34] = kurtosis;
    features[35] = iqr;
    

}

void CF_fill_features_arr(float* acc_X, float* acc_Y, float* acc_Z,
                          float* gyr_X, float* gyr_Y, float* gyr_Z,
                          uint16_t n_samples,float *features)
{
   float *axis[6] = {acc_X, acc_Y, acc_Z, gyr_X, gyr_Y, gyr_Z};
   
   for(uint16_t i = 0; i < 6; i ++)
   {
    CF_fill_raw_features(&features[i*13], axis[i], n_samples);
   }
   for(uint16_t i = 0; i < 6; i ++)
   {
    CF_fill_frequency_features(&features[i*36 + 78], axis[i], n_samples);
   }
   //CF_standardize_features(features);
}