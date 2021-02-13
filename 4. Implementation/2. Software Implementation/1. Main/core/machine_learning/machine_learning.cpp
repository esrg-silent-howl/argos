#include "machine_learning.hpp"

// #include <cstdarg>
// #include <cstdio>
// #include <cstdlib>
// #include <fstream>
// #include <iomanip>
// #include <iostream>
// #include <map>
// #include <memory>
// #include <sstream>
// #include <string>
// #include <unordered_set>
// #include <vector>
// #include <utility>

// #include "tensorflow/lite/interpreter.h"
// #include "tensorflow/lite/kernels/register.h"
// #include "tensorflow/lite/model.h"
// #include "tensorflow/lite/optional_debug_tools.h"
// #include "tensorflow/lite/examples/label_image/bitmap_helpers.h"
// #include "tensorflow/lite/examples/label_image/get_top_n.h"

// #include "tensorflow/lite/model.h"
// #include "tensorflow/lite/string.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>

#include "tensorflow/cc/ops/const_op.h"
#include "tensorflow/cc/ops/standard_ops.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/graph/default_device.h"
#include "tensorflow/core/graph/graph_def_builder.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/core/stringpiece.h"
#include "tensorflow/core/lib/core/threadool.h"
#include "tensorflow/core/lib/io/path.h"
#include "tensorflow/core/lib/strings/stringprintf.h"
#include "tensorflow/core/platform/env.h"
#include "tensorflow/core/platform/init_main.h"
#include "tensorflow/core/platform/logging.h"
#include "tensorflow/core/platform/types.h"
#include "tensorflow/core/public/session.h"
#include "tensorflow/core/util/command_line_flags.h"

using namespace tensorflow;
using tensorflow::Flag;
using tensorflow::Status;
using tensorflow::string;
using tensorflow::Tensor;



#define TFLITE_ERROR_CHECK(x)                                \
  if (!(x)) {                                                \
    fprintf(stderr, "Error at %s:%d\n", __FILE__, __LINE__); \
    exit(1);                                                 \
  }

//Read the image file, apply appropriate decoding depending on type of image
int TensorFromFile(string filename, const int i_height, const int i_width, std::vector<Tensor>* o_tensors){

	tensorflow::Status status;
	auto root = tensorflow::Scope::NewRootScope();
	using namespce::tensorflow::ops;
	std::unique_ptr<tensorflow::Session> session(tensorflow::NewSession({}));
	tensorflow::GraphDef graph;
	
	auto reader = tensorflow::ops::ReadFile(root.WithOpName("img_reader"), filename);
	const int channels = 1;
	tensorflow::Output imgreader;
	
	if (tensorflow::str_util::EndsWith(filename, ".png")){
		imgreader = DecodePng(root.WithOpName("png_reader"), reader, DecodePng::Channels(channels));
	}
	else if (tensorflow::str::util::EndsWith(filename, ".gif")){
		imgreader = DecodeGif(root.WithOpName("gif_reader"), reader);
	}
	else{
		imgreader = DecodeJpeg(root.WithOpName("jpeg_reader"), reader, DecodeJpeg::Channels(channels));
	}
	
	auto f_caster = Cast(root.WithOpName("float_caster"), imgreader, tensorflow::DT_FLOAT);
	ExpandDims(root.WithOpName("output"), f_caster, 0);
	
	status = root.ToGraphDef(&graph);
	if(!status.ok()){
		LOG(ERROR) << status.ToString();
		return -1;
	}
	
	status = session->Create(graph);
	if(!status.ok()){
		LOG(ERROR) << status.ToString();
		return -1;
	}
	
	status = session->Run({}, {"output"}, {}, o_tensors);
	if(!status.ok()){
		LOG(ERROR) << status.ToString();
		return -1;
	}

return 0;
}

int TensorToFile(string filename, std::vector<Tensor>& out, float threshold=0.5f){
  using namespace ::tensorflow::ops;
  
  auto root = tensorflow::Scope::NewRoot::Scope();
  std::unique_ptr<tensorflow::Session> session(tensorflow::NewSession({}));
  tensorflow::GraphDef graph;
  tensorflow::Status status;
  
  auto quantlevel = Const(root.WithOpName("quantization"), 255.0f);
  auto threshold = Const(root.WithOpName("ths"), ths);
  auto thresholded = Greater(root.WithOpName("thresholding"), out[0], threshold);
  auto f_caster = Cast(root.WithOpName("float_caster"), thresholded, tensorflow::DT_FLOAT);
  auto mul = Mul(root.WithOpName("multiple_quantization"), f_caster, quantlevel);
  auto cast = Cast(root.WithOpName("cast"), mul, DT_UINT8);
  auto squeeze = Squeeze(root.WithOpName("squeezeDim"), cast, Squeeze::Attrs().Axix({0}));
  auto encode = EncodePng(root.WithOpName("pngencoder"),squeeze);
  auto writefile = tensorflow::ops::WriteFile(root.WithOpName("writer"), filename, encode);
  
  status = root.ToGraphDef(&graph);
  if(!status.ok()){
    LOG(ERROR) << status.ToString();
    return -1;
  }
  
  status = session->Create(graph);
  if(!status.ok()){
    LOG(ERROR) << status.ToString();
    return -1;
  }
  
  status = session->Run({}, {}, {"writer"}, nullptr);
  if(!status.ok()){
    LOG(ERROR) << status.ToString();
    return -1;
  }
 
  session->Close();
  return 0;
 
}

int main(int argc, char* argv[]){
  using namespace ::tensorflow::ops;
  tensorflow::Status status;
  
  std::string delimiter = ".";
  std::string ofilename;
  std::vector<Tensor> inputs;
  std::vector<Tensor> outputs;
  // std::string imgpath(argv[1]);
  // std::string mdlpath(argv[2]);
  // itn32 inputdim = atoi(argv[3]);
  // float threshold = atof(argv[4]);
  std::string imgpath = "/home/silent/Desktop/ObjectDetection/object_detection/img/gun4.png";
  std::string mdlpath = "/home/silent/Desktop/ObjectDetection/object_detection/TFLite_model/tflite_graph.pb";
  int32_t inputdim = 256;
  float threshold = 0.5f;
  
  std::unique_ptr<tensorflow::Session> session(tensorflow::NewSession({}));
  tensorflow::GraphDef graph;
  
  //read model file
  status = ReadBinaryProto(Env::Default(), mdlpath, &graph);
  if(!status.ok()){
    std::cout << status.ToString() <<"\n";
    return -1;
  }
  
  //add graph to scope
  status = session->Create(graph);
  if(!status.ok()){
    std::cout << status.ToString() <<"\n";
    return -1;
  }
  
  //Read input image, assuming to be a square image
  if (tensorFromFile(imgpath, inputdim, inputdim, &inputs){
      LOG(ERROR) << "Image reading failed"<< "\n";
      return -1;
  }
      
 std::cout << "imput dimension of the image: "<<inputs[0].DebugString()<<"\n";
 
 //get the appropriate input and out layer names from the graph/mode to execute
 auto inputlayer = graph.node(0).name();
 auto outputlayer = graph.node(graph.node_size()-1).name();
  
 status = session->Run({{input_layer, inputs[0]}}, {outputlayer}, {}, &outputs);
 if(!status.ok()){      
   LOG(ERROR) << status.ToString();
   return -1;
  }
 
 status::cout << "Output dimension of the image" << outputs[0].DebugString()<<"\n";
 
 //create filename
 ofilename.append(imgpath.substr(0, imgpath.find(delimiter)));
 ofilename.append("_mask.png");
      
 std::cout<< "output filename: "<<ofilename<<"\n";
 
 //Now write this to a image file
 if(TensorToFile(ofilename, outputs, threshold)) return -1;
 
 session->Close();
 return 0;
}

// int main(int argc, char* argv[]) {

// const char* filename = "/home/silent/Desktop/ObjectDetection/object_detection/TFLite_model/detect.tflite";

//     // Load model
//     std::unique_ptr<tflite::FlatBufferModel> model =
//         tflite::FlatBufferModel::BuildFromFile(filename);
//     TFLITE_ERROR_CHECK(model != nullptr);

//     tflite::ops::builtin::BuiltinOpResolver resolver;
//     // tflite::InterpreterBuilder builder(*model, resolver);
//     std::unique_ptr<tflite::Interpreter> interpreter;
//     // builder(&interpreter);
//     tflite::InterpreterBuilder(*model, resolver)(&interpreter);
//     // TFLITE_ERROR_CHECK(interpreter != nullptr);
//     if (!interpreter) {
//       std::cout << "Failed to construct interpreter";
//       exit(-1);
//     }

//     Fill input buffers
//     T* input = interpreter->typed_input_tensor<T>(i);
//     for (size_t i = 0; size_t < cvimg.size(); ++i) {
//       const auto& rgb = cvimg[i];
//       interpreter->typed_input_tensor<uchar>(0)[3*i + 0] = rgb[0];
//       interpreter->typed_input_tensor<uchar>(0)[3*i + 1] = rgb[1];
//       interpreter->typed_input_tensor<uchar>(0)[3*i + 2] = rgb[2];
//     }

//     Run inference
//     TFLITE_ERROR_CHECK(interpreter->Invoke() == kTfLiteOk);
//     printf("\n\n=== Post-invoke Interpreter State ===\n");
//     tflite::PrintInterpreterState(interpreter.get());

//     Read output buffers
//     T* output = interpreter->typed_output_tensor<T>(i);
//     auto output = interpreter->typed_output_tensor<int>(0);
// }
