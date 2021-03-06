#include <cstdlib>
#include "opencv/cv.h"
#include "opencv/ml.h"
#include <vector>
#include <fstream>

using namespace std;
using namespace cv;

int main(int argc, char** argv) {

//Read the data from csv file
CvMLData cvml;
cvml.read_csv("char_datasetNM1.csv");
//Indicate which column is the response
cvml.set_response_idx(0);


//Select 90% for the training 
CvTrainTestSplit cvtts(0.9f, true);
//Assign the division to the data
cvml.set_train_test_split(&cvtts);

CvBoost boost;

ifstream ifile("./trained_classifierNM1.xml");
if (ifile) 
{
	//The file exists, so we don't want to train 
	printf("Found trained_boost_char.xml file, remove it if you want to retrain with new data ... \n");
	boost.load("./trained_classifierNM1.xml", "boost");
} else {
	//Train with 100 features
	printf("Training ... \n");
	boost.train(&cvml, CvBoostParams(CvBoost::REAL, 100, 0, 1, false, 0), false);
}

//Calculate the test and train errors
std::vector<float> train_responses, test_responses;
float fl1 = boost.calc_error(&cvml,CV_TRAIN_ERROR,&train_responses);
float fl2 = boost.calc_error(&cvml,CV_TEST_ERROR,&test_responses);
printf("Error train %f \n", fl1);
printf("Error test %f \n", fl2);


//Try a char
static const float arr[] = {0,1.063830,0.083372,0.000000,2.000000};
vector<float> sample (arr, arr + sizeof(arr) / sizeof(arr[0]) );
float prediction = boost.predict( Mat(sample), Mat(), Range::all(), false, false );
float votes      = boost.predict( Mat(sample), Mat(), Range::all(), false, true );

printf("\n The char sample is predicted as: %f (with number of votes = %f)\n", prediction,votes);
printf(" Class probability (using Logistic Correction) is P(r|character) = %f\n", (float)1-(float)1/(1+exp(-2*votes)));

//Try a NONchar
static const float arr2[] = {0,2.000000,0.235702,0.000000,2.000000};
vector<float> sample2 (arr2, arr2 + sizeof(arr2) / sizeof(arr2[0]) );
prediction = boost.predict( Mat(sample2), Mat(), Range::all(), false, false );
votes      = boost.predict( Mat(sample2), Mat(), Range::all(), false, true );

printf("\n The non_char sample is predicted as: %f (with number of votes = %f)\n", prediction,votes);
printf(" Class probability (using Logistic Correction) is P(r|character) = %f\n\n", (float)1-(float)1/(1+exp(-2*votes)));

// Save the trained classifier
boost.save("./trained_classifierNM1.xml", "boost");

return EXIT_SUCCESS;
}
