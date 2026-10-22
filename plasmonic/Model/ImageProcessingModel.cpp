
#include "ImageProcessingModel.h"

ImageProcessingModel::ImageProcessingModel(QObject* parent) : QObject(parent)
{
}

void ImageProcessingModel::processFrame(const QByteArray& frame)
{
    constexpr int width = 1920;
    constexpr int height = 1080;
    constexpr int stride = width * 5 / 4;

    cv::Mat raw(height, width, CV_16UC1);

    const auto* source = reinterpret_cast<const uint8_t*>(frame.constData());

    for (int y = 0; y < height; ++y)
    {
        const uint8_t* row = source + y * stride;
        uint16_t* pixels = raw.ptr<uint16_t>(y);

        for (int x = 0; x < width; x += 4)
        {
            const int offset = x * 5 / 4;
            const uint8_t low = row[offset + 4];

            for (int i = 0; i < 4; ++i)
            {
                pixels[x + i] = (static_cast<uint16_t>(row[offset + i]) << 2) | ((low >> (2 * i)) & 0x03);
            }
        }
    }

    cv::Mat preview;
    raw.convertTo(preview, CV_8UC1, 1.0 / 4.0);

    Q_EMIT frameReady(preview);
}
