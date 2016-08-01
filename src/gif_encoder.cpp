/*
 * Copyright (C) 2014-2016 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus Quest Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "editor_exception.h"
#include "gif_encoder.h"
#include <gif_lib.h>
#include <QApplication>

namespace SolarusEditor {

/**
 * @brief Encodes a sprite direction in a gif file.
 * @param filename The filename.
 * @param image The source image.
 * @param frames The frames rect.
 * @param loop Whether the animation must loop.
 * @param frame_delay The frame delay.
 * @param transparent_color The index of the transparent color.
 * @throws EditorException In case of error.
 */
void GifEncoder::encode_sprite_direction(
  const QString& filename, QImage& image, const QList<QRect>& frames,
  bool loop, int frame_delay, int transparent_color) {

  // Check the image format.
  if (image.format() != QImage::Format_Indexed8) {
    image = image.convertToFormat(
      QImage::Format_Indexed8, Qt::ThresholdDither | Qt::PreferDither);
  }

  // Open the gif file.
  int error = 0;
  GifFileType* file =
    EGifOpenFileName(filename.toStdString().c_str(), false, &error);

  if (error) {
    throw EditorException(
      QApplication::tr("Cannot open file '%1' for writing").arg(filename));
  }

  if (loop || frame_delay > 0 || transparent_color >= 0) {
    // Set the version to Gif89 to allow loop, animation and transparency.
    EGifSetGifVersion(file, true);
  }

  // Put the global datas.
  QSize size = frames.first().size();
  put_global_data(file, image, size.width(), size.height(), loop);

  // Put the frames.
  put_frames_data(file, image, frames, frame_delay, transparent_color);

  // Close the gif file.
  EGifCloseFile(file, &error);
}

/**
 * @brief Puts a global sprite direction animation descriptor in a gif file.
 * @param file The gif file where to puts the global descriptor.
 * @param image The source image of the animation.
 * @param width The width of the frames.
 * @param height The height of the frames.
 * @param loop Whether the animation must loop.
 */
void GifEncoder::put_global_data(
  GifFileType* file, const QImage& image, int width, int height, bool loop) {

  // Compute the color resolution (must be a power of 2).
  int color_count = image.colorCount();
  int color_resolution = 2;
  while (color_resolution < color_count) {
    color_resolution <<= 1;
  }

  // Make the color map.
  ColorMapObject* color_map = GifMakeMapObject(color_resolution, nullptr);

  for (int i = 0; i < color_count; ++i) {
    QRgb rgb = image.color(i);
    color_map->Colors[i].Red = static_cast<GifByteType>(qRed(rgb));
    color_map->Colors[i].Green = static_cast<GifByteType>(qGreen(rgb));
    color_map->Colors[i].Blue = static_cast<GifByteType>(qBlue(rgb));
  }

  for (int i = color_count; i < color_resolution; ++i) {
    color_map->Colors[i].Red = 0;
    color_map->Colors[i].Green = 0;
    color_map->Colors[i].Blue = 0;
  }

  // Put the screen descriptor.
  EGifPutScreenDesc(file, width, height, color_map->ColorCount, 0, color_map);

  if (loop) {
    // Put the looping application extension.
    static const GifByteType data[] = { 1, 0, 0 };
    EGifPutExtensionLeader(file, APPLICATION_EXT_FUNC_CODE);
    EGifPutExtensionBlock(file, 11, "NETSCAPE2.0");
    EGifPutExtensionBlock(file, 3, data);
    EGifPutExtensionTrailer(file);
  }

  // Free the color map.
  GifFreeMapObject(color_map);
}

/**
 * @brief Puts the sprite direction frames data in a gif file.
 * @param file The gif file where to put the datas.
 * @param image The source image of the animation.
 * @param frames The frames rect.
 * @param frame_delay The frame delay of the animation.
 * @param transparent_color The index of the color transparency.
 */
void GifEncoder::put_frames_data(
  GifFileType* file, QImage& image, const QList<QRect>& frames,
  int frame_delay, int transparent_color) {

  GifByteType extension[4];
  bool need_extension = frame_delay > 0 || transparent_color >= 0;

  if (need_extension) {
    // Make a graphics control extension.
    GraphicsControlBlock control_block;
    control_block.DisposalMode = DISPOSE_BACKGROUND;
    control_block.UserInputFlag = false;
    control_block.DelayTime = frame_delay / 10;
    control_block.TransparentColor = transparent_color;
    EGifGCBToExtension(&control_block, extension);
  }

  for (const QRect& frame: frames) {

    if (need_extension) {
      // Put the graphics control extension.
      EGifPutExtension(file, GRAPHICS_EXT_FUNC_CODE, 4, extension);
    }

    // Put the image descriptor.
    EGifPutImageDesc(file, 0, 0, frame.width(), frame.height(), false, nullptr);

    // Put the image data.
    for (int i = 0; i < frame.height(); ++i) {
      GifPixelType* line = image.scanLine(i + frame.y()) + frame.x();
      EGifPutLine(file, line, frame.width());
    }
  }
}

}