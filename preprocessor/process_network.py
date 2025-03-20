# @author       Jiawei Lu (jiaweil9@asu.edu)
# @time         1/14/23 6:52 PM
# @desc         [script description]


import pandas as pd
import os
from shapely import geometry, wkt
import osm2gmns as og


network_folder = '/Users/jiawei/Desktop/network'


# ================= link geometry ================= #
# link_df = pd.read_csv(os.path.join(network_folder, 'link_orig.csv'))
# geometry_df = pd.read_csv(os.path.join(network_folder, 'geometry.csv'))
#
# geometry_dict = {}
# for i in range(len(geometry_df)):
#     geometry_dict[geometry_df.loc[i,'geometry_id']] = wkt.loads(geometry_df.loc[i,'geometry'])
#
# link_df['geometry'] = link_df.apply(lambda x:geometry_dict[x['geometry_id']] if x['dir_flag'] == 1 else geometry.LineString(geometry_dict[x['geometry_id']].coords[::-1]), axis=1)
#
# link_df.to_csv(os.path.join(network_folder, 'link.csv'), index=False)



net = og.loadNetFromCSV(network_folder, 'node.csv', 'link.csv', 'movement.csv', 'segment.csv')
og.buildMultiResolutionNets(net)
og.outputNetToCSV(net,os.path.join(network_folder,'mrm'))


