guint8 *in_data=in_map.data;
		guint8 *out_data=out_map.data;
		guint8 *y_data = in_map.data;
                guint8 *u_data = in_map.data + pre_width * pre_height;
                guint8 *v_data = in_map.data + pre_width * pre_height;
                guint8 *oy_data = out_map.data;
                guint8 *ou_data = out_map.data + org_width * org_height;
                guint8 *ov_data = out_map.data + org_width * org_height;
		
		for (int y = 0; y < org_height; y++) {
                    for (int x = 0; x < org_width; x++) {
                    
                         guint8 y_pixel = y_data[y * org_width + x];
                         guint8 u_pixel = u_data[(y / 2) * org_width + 2 * (x / 2)];
                         guint8 v_pixel = v_data[(y / 2) * org_width + 2 * (x / 2) + 1];
                         
                         
                         oy_data[y * org_width + x]=y_pixel;
                         ou_data[(y / 2) * org_width + 2 * (x / 2)] = u_pixel;
                         ov_data[(y / 2) * org_width + 2 * (x / 2) + 1] = v_pixel;
                         

            // Process y_pixel, u_pixel, v_pixel as needed
                    }
               }
