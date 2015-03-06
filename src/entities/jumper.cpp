/*
 * Copyright (C) 2014-2015 Christopho, Solarus - http://www.solarus-games.org
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
#include "entities/jumper.h"

/**
 * @brief Constructor.
 * @param map The map containing the entity.
 * @param entity The entity data to represent.
 */
Jumper::Jumper(MapModel& map, const Solarus::EntityData& entity) :
  EntityModel(map, entity) {

  Q_ASSERT(entity.get_type() == EntityType::JUMPER);

  DrawShapeInfo info;
  info.enabled = true;
  info.background_color = QColor(48, 184, 208);
  info.between_border_color = QColor(144, 224, 240);
  set_draw_shape_info(info);
}
