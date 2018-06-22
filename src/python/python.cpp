#define PY_ARRAY_UNIQUE_SYMBOL pbcvt_ARRAY_API

#include <boost/python.hpp>
#include "pyboostcvconverter.hpp"

extern int docartoonize(const cv::Mat &input, cv::Mat &output, int width, int height);

namespace pycartoon {
    using namespace boost::python;

    cv::Mat cppcartoonize(const cv::Mat& frame, int width, int height)
    {
        cv::Mat output;

        if (docartoonize(frame, output, width, height) != 0) {
            PyErr_SetString(PyExc_TypeError,
                            "docartoonize failed!");
        }

        return output;
    }

#if (PY_VERSION_HEX >= 0x03000000)

    static void *init_ar() {
#else
        static void init_ar(){
#endif
        Py_Initialize();

        import_array();
        return NUMPY_IMPORT_ARRAY_RETVAL;
    }

    BOOST_PYTHON_MODULE (pycartoon) {
        //using namespace XM;
        init_ar();

        //initialize converters
        to_python_converter<cv::Mat,
                pbcvt::matToNDArrayBoostConverter>();
        pbcvt::matFromNDArrayBoostConverter();

        def("cppcartoonize", cppcartoonize);
    }

} //end namespace pycartoon
