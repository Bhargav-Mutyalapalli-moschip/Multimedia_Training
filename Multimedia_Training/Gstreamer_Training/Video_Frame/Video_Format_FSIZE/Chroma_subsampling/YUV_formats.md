# About YUV formats

First of all: [YUV pixel formats](https://www.fourcc.org/yuv.php) and [Recommended 8-Bit YUV Formats for Video Rendering](https://msdn.microsoft.com/en-us/library/windows/desktop/dd206750(v=vs.85).aspx). Chromium's source code contains good documentation about those formats too: [chromium/src/media/base/video_types.h](https://cs.chromium.org/chromium/src/media/base/video_types.h) and [chromium/src/media/base/video_frame.cc](https://cs.chromium.org/chromium/src/media/base/video_frame.cc) (search for `RequiresEvenSizeAllocation()`, `NumPlanes()` and those kinds of functions).

### YUV?

You can think of an image as a superposition of several _planes_ (or layers in a more natural language). YUV formats have three planes: `Y`, `U`, and `V`.

`Y` is the _luma_ plane, and can be seen as the image as grayscale. `U` and `V` are reffered to as the _chroma_ planes, which are basically the colours. All the YUV formats have these three planes, and differ by the different orderings of them.

Sometimes the term `YCrCb` is used. It's essentially the same thing: `Y`, `Cr`, `Cb` respectively refer to `Y`, `U`, `V` although `Cr` and `Cb` are sometimes used when speaking of the components of `U` and `V` (that is `U` = Cr<sub>1</sub>Cr<sub>2</sub>…Cr<sub>n</sub> and `V` = Cb<sub>1</sub>Cb<sub>2</sub>…Cb<sub>n</sub>).

### 420, 422, (…), and subsampling

The chroma planes (`U` and `V`) are subsampled. This means there is less information in the chroma planes than in the luma plane. This is because the human eye is less sensitive to colours than luminance; so we can just gain space by keeping less information about colours (chroma planes) than luminance (luma plane, `Y`).

The subsampling is expressed as a three part ratio: `J:a:b` (e.g. `4:2:0`). This ratio makes possible to get the size of the planes relative to each others. Refer to the Wikipedia article "[Chroma subsampling](https://en.wikipedia.org/wiki/Chroma_subsampling#Sampling_systems_and_ratios)" for more information.

Basically the chroma planes are often shorter than the luma plane. Most commonly, the ratio is `length(Y) / n = length(U) = length(V)` where `n` is 1, 2, 4, …

### Packed, Planar, and Semi-planar

YUV formats are either:
1. _Packed_ (or _interleaved_)
2. _Planar_ (the names of those formats often end with "_p_")
3. _Semi-planar_ (the names of those formats often end with "_sp_")

Those terms define how the planes are ordered in the format. In the memory:
1. Packed means the components of `Y`, `U`, and `V` are interleaved. For instance: Y<sub>1</sub>U<sub>1</sub>Y<sub>2</sub>V<sub>1</sub>Y<sub>3</sub>U<sub>2</sub>Y<sub>4</sub>V<sub>2</sub>…Y<sub>n-1</sub>U<sub>n/2</sub>Y<sub>n</sub>V<sub>n/2</sub>.
2. Planar means the components of `Y`, `U`, and `V` are respectively grouped together. For instance: Y<sub>1</sub>Y<sub>2</sub>…Y<sub>n</sub>U<sub>1</sub>U<sub>2</sub>…U<sub>n/2</sub>V<sub>1</sub>V<sub>2</sub>…V<sub>n/2</sub>.
3. Semi-planar means the components of `Y` are grouped together, and the components of `U` and `V` are interleaved. For instance: Y<sub>1</sub>Y<sub>2</sub>…Y<sub>n</sub>U<sub>1</sub>V<sub>1</sub>U<sub>2</sub>V<sub>2</sub>…U<sub>n/2</sub>V<sub>n/2</sub>

Semi-planar formats are sometimes put in the Planar familly.

## Some YUV formats

The following formats are described in the picture:

- YV12
- I420 = IYUV = YUV420p (sometimes YUV420p can refer to YV12)
- NV21
- NV12 = YUV420sp (sometimes YUV420sp can refer to NV21)
