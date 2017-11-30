#!/bin/bash
#
#$ -S /bin/bash
#$ -N omptest
#$ -pe openmp 16
#$ -l h_vmem=2G

export OMP_NUM_THREADS=7  # gets number from -pe
### for performance reasons, you might want to use some agressive scheduling ###
export OMP_WAIT_POLICY=active
export OMP_DYNAMIC=false
export OMP_PROC_BIND=true


#rm -r test_snow0
#mkdir test_snow0
#./poff_no_graph -l test_snow.conf -s falling_cube_cylinderh.sc -e test_snow0/test -es 2 -stop 2000
#
#rm -r test_snow_ap1
#mkdir test_snow_ap1
#./poff_no_graph -l test_snow_ap1.conf -s falling_cube_cylinderr.sc -e test_snow_ap1/testr_ -es 2 -stop 2000
#./poff_no_graph -l test_snow_ap1.conf -s falling_cube_cylinderh.sc -e test_snow_ap1/testh_ -es 2 -stop 2000
#./poff_no_graph -l test_snow_ap1.conf -s falling_cube_cylinderd.sc -e test_snow_ap1/testd_ -es 2 -stop 2000
#./poff_no_graph -l test_snow_ap1.conf -s falling_cube_cylinderv.sc -e test_snow_ap1/testv_ -es 2 -stop 2000
#
#rm -r test_snow_ap2
#mkdir test_snow_ap2
#./poff_no_graph -l test_snow_ap2.conf -s falling_cube_cylinderr.sc -e test_snow_ap2/testr_ -es 2 -stop 2000
#./poff_no_graph -l test_snow_ap2.conf -s falling_cube_cylinderh.sc -e test_snow_ap2/testh_ -es 2 -stop 2000
#./poff_no_graph -l test_snow_ap2.conf -s falling_cube_cylinderd.sc -e test_snow_ap2/testd_ -es 2 -stop 2000
#./poff_no_graph -l test_snow_ap2.conf -s falling_cube_cylinderv.sc -e test_snow_ap2/testv_ -es 2 -stop 2000
#
#rm -r test_sand0
#mkdir test_sand0
#./poff_no_graph -l test_sand.conf -s columnh.sc -e test_sand0/test -es 2 -stop 2000
#
#rm -r test_sand_ap1
#mkdir test_sand_ap1
#./poff_no_graph -l test_sand_ap1.conf -s columnr.sc -e test_sand_ap1/testr_ -es 2 -stop 2000
#./poff_no_graph -l test_sand_ap1.conf -s columnh.sc -e test_sand_ap1/testh_ -es 2 -stop 2000
#./poff_no_graph -l test_sand_ap1.conf -s columnd.sc -e test_sand_ap1/testd_ -es 2 -stop 2000
#./poff_no_graph -l test_sand_ap1.conf -s columnv.sc -e test_sand_ap1/testv_ -es 2 -stop 2000
#
#rm -r test_sand_ap2
#mkdir test_sand_ap2
#./poff_no_graph -l test_sand_ap2.conf -s columnr.sc -e test_sand_ap2/testr_ -es 2 -stop 2000
#./poff_no_graph -l test_sand_ap2.conf -s columnh.sc -e test_sand_ap2/testh_ -es 2 -stop 2000
#./poff_no_graph -l test_sand_ap2.conf -s columnd.sc -e test_sand_ap2/testd_ -es 2 -stop 2000
#./poff_no_graph -l test_sand_ap2.conf -s columnv.sc -e test_sand_ap2/testv_ -es 2 -stop 2000

#rm -r test_goo0
#mkdir test_goo0
#./poff_no_graph -l test_goo.conf -s columnh.sc -e test_goo0/test -es 2 -stop 5000
#
##rm -r test_goo_ap1
#mkdir test_goo_ap1
#./poff_no_graph -l test_goo_ap1.conf -s columnr.sc -e test_goo_ap1/testr_ -es 5 -stop 10000
#./poff_no_graph -l test_goo_ap1.conf -s columnh.sc -e test_goo_ap1/testh_ -es 5 -stop 10000
#./poff_no_graph -l test_goo_ap1.conf -s columnd.sc -e test_goo_ap1/testd_ -es 5 -stop 10000
#./poff_no_graph -l test_goo_ap1.conf -s columnv.sc -e test_goo_ap1/testv_ -es 5 -stop 10000
#
##rm -r test_goo_ap2
#mkdir test_goo_ap2
#./poff_no_graph -l test_goo_ap2.conf -s columnr.sc -e test_goo_ap2/testr_ -es 5 -stop 10000
#./poff_no_graph -l test_goo_ap2.conf -s columnh.sc -e test_goo_ap2/testh_ -es 5 -stop 10000
#./poff_no_graph -l test_goo_ap2.conf -s columnd.sc -e test_goo_ap2/testd_ -es 5 -stop 10000
#./poff_no_graph -l test_goo_ap2.conf -s columnv.sc -e test_goo_ap2/testv_ -es 5 -stop 10000

#rm -r sand
#mkdir sand
#./poff_no_graph -l sand0.conf -s columnd.sc -e sand/test0_ -es 2 -stop 2000
#./poff_no_graph -l sand-1.conf -s columnd.sc -e sand/test-1_ -es 2 -stop 2000
#./poff_no_graph -l sand-2.conf -s columnd.sc -e sand/test-2_ -es 2 -stop 2000
#./poff_no_graph -l sand-3.conf -s columnd.sc -e sand/test-3_ -es 2 -stop 2000
#./poff_no_graph -l sand-4.conf -s columnd.sc -e sand/test-4_ -es 2 -stop 2000
#./poff_no_graph -l sand1.conf -s columnd.sc -e sand/test1_ -es 2 -stop 2000
#./poff_no_graph -l sand2.conf -s columnd.sc -e sand/test2_ -es 2 -stop 2000
#./poff_no_graph -l sand3.conf -s columnd.sc -e sand/test3_ -es 2 -stop 2000
#./poff_no_graph -l sand4.conf -s columnd.sc -e sand/test4_ -es 2 -stop 2000
#mkdir sandv
#./poff_no_graph -l sand0.conf -s columnv.sc -e sandv/test0_ -es 2 -stop 2000
#./poff_no_graph -l sand-1.conf -s columnv.sc -e sandv/test-1_ -es 2 -stop 2000
#./poff_no_graph -l sand-2.conf -s columnv.sc -e sandv/test-2_ -es 2 -stop 2000
#./poff_no_graph -l sand-3.conf -s columnv.sc -e sandv/test-3_ -es 2 -stop 2000
#./poff_no_graph -l sand-4.conf -s columnv.sc -e sandv/test-4_ -es 2 -stop 2000
#./poff_no_graph -l sand1.conf -s columnv.sc -e sandv/test1_ -es 2 -stop 2000
#./poff_no_graph -l sand2.conf -s columnv.sc -e sandv/test2_ -es 2 -stop 2000
#./poff_no_graph -l sand3.conf -s columnv.sc -e sandv/test3_ -es 2 -stop 2000
#./poff_no_graph -l sand4.conf -s columnv.sc -e sandv/test4_ -es 2 -stop 2000

#mkdir sandh
#./poff_no_graph -l sand0.conf -s columnh.sc -e sandh/test0_ -es 2 -stop 2000
#./poff_no_graph -l sand-1.conf -s columnh.sc -e sandh/test-1_ -es 2 -stop 2000
#./poff_no_graph -l sand-2.conf -s columnh.sc -e sandh/test-2_ -es 2 -stop 2000
#./poff_no_graph -l sand-3.conf -s columnh.sc -e sandh/test-3_ -es 2 -stop 2000
#./poff_no_graph -l sand-4.conf -s columnh.sc -e sandh/test-4_ -es 2 -stop 2000
#./poff_no_graph -l sand1.conf -s columnh.sc -e sandh/test1_ -es 2 -stop 2000
#./poff_no_graph -l sand2.conf -s columnh.sc -e sandh/test2_ -es 2 -stop 2000
#./poff_no_graph -l sand3.conf -s columnh.sc -e sandh/test3_ -es 2 -stop 2000
#./poff_no_graph -l sand4.conf -s columnh.sc -e sandh/test4_ -es 2 -stop 2000



#rm -r sand_cyl
#mkdir sand_cyl
#./poff_no_graph -l sand0.conf -s falling_cube_cylinderd.sc -e sand_cyl/test0_ -es 2 -stop 2000
#./poff_no_graph -l sand-1.conf -s falling_cube_cylinderd.sc -e sand_cyl/test-1_ -es 2 -stop 2000
#./poff_no_graph -l sand-2.conf -s falling_cube_cylinderd.sc -e sand_cyl/test-2_ -es 2 -stop 2000
#./poff_no_graph -l sand-3.conf -s falling_cube_cylinderd.sc -e sand_cyl/test-3_ -es 2 -stop 2000
#./poff_no_graph -l sand-4.conf -s falling_cube_cylinderd.sc -e sand_cyl/test-4_ -es 2 -stop 2000
#./poff_no_graph -l sand1.conf -s falling_cube_cylinderd.sc -e sand_cyl/test1_ -es 2 -stop 2000
#./poff_no_graph -l sand2.conf -s falling_cube_cylinderd.sc -e sand_cyl/test2_ -es 2 -stop 2000
#./poff_no_graph -l sand3.conf -s falling_cube_cylinderd.sc -e sand_cyl/test3_ -es 2 -stop 2000
#./poff_no_graph -l sand4.conf -s falling_cube_cylinderd.sc -e sand_cyl/test4_ -es 2 -stop 2000
#mkdir sandv_cyl
#./poff_no_graph -l sand0.conf -s falling_cube_cylinderv.sc -e sandv_cyl/test0_ -es 2 -stop 2000
#./poff_no_graph -l sand-1.conf -s falling_cube_cylinderv.sc -e sandv_cyl/test-1_ -es 2 -stop 2000
#./poff_no_graph -l sand-2.conf -s falling_cube_cylinderv.sc -e sandv_cyl/test-2_ -es 2 -stop 2000
#./poff_no_graph -l sand-3.conf -s falling_cube_cylinderv.sc -e sandv_cyl/test-3_ -es 2 -stop 2000
#./poff_no_graph -l sand-4.conf -s falling_cube_cylinderv.sc -e sandv_cyl/test-4_ -es 2 -stop 2000
#./poff_no_graph -l sand1.conf -s falling_cube_cylinderv.sc -e sandv_cyl/test1_ -es 2 -stop 2000
#./poff_no_graph -l sand2.conf -s falling_cube_cylinderv.sc -e sandv_cyl/test2_ -es 2 -stop 2000
#./poff_no_graph -l sand3.conf -s falling_cube_cylinderv.sc -e sandv_cyl/test3_ -es 2 -stop 2000
#./poff_no_graph -l sand4.conf -s falling_cube_cylinderv.sc -e sandv_cyl/test4_ -es 2 -stop 2000
#
#mkdir sandh_cyl
#./poff_no_graph -l sand0.conf -s falling_cube_cylinderh.sc -e sandh_cyl/test0_ -es 2 -stop 2000
#./poff_no_graph -l sand-1.conf -s falling_cube_cylinderh.sc -e sandh_cyl/test-1_ -es 2 -stop 2000
#./poff_no_graph -l sand-2.conf -s falling_cube_cylinderh.sc -e sandh_cyl/test-2_ -es 2 -stop 2000
#./poff_no_graph -l sand-3.conf -s falling_cube_cylinderh.sc -e sandh_cyl/test-3_ -es 2 -stop 2000
#./poff_no_graph -l sand-4.conf -s falling_cube_cylinderh.sc -e sandh_cyl/test-4_ -es 2 -stop 2000
#./poff_no_graph -l sand1.conf -s falling_cube_cylinderh.sc -e sandh_cyl/test1_ -es 2 -stop 2000
#./poff_no_graph -l sand2.conf -s falling_cube_cylinderh.sc -e sandh_cyl/test2_ -es 2 -stop 2000
#./poff_no_graph -l sand3.conf -s falling_cube_cylinderh.sc -e sandh_cyl/test3_ -es 2 -stop 2000
#./poff_no_graph -l sand4.conf -s falling_cube_cylinderh.sc -e sandh_cyl/test4_ -es 2 -stop 2000

#mkdir thin_col/
#./poff_no_graph -l sand0.conf -s thin_columnv.sc -e thin_col/test0_ -es 2 -stop 5000
#./poff_no_graph -l sand-1.conf -s thin_columnv.sc -e thin_col/test-1_ -es 2 -stop 5000
#./poff_no_graph -l sand-2.conf -s thin_columnv.sc -e thin_col/test-2_ -es 2 -stop 5000
#./poff_no_graph -l sand-3.conf -s thin_columnv.sc -e thin_col/test-3_ -es 2 -stop 5000
#./poff_no_graph -l sand-4.conf -s thin_columnv.sc -e thin_col/test-4_ -es 2 -stop 5000
#./poff_no_graph -l sand1.conf -s thin_columnv.sc -e thin_col/test1_ -es 2 -stop 5000
#./poff_no_graph -l sand2.conf -s thin_columnv.sc -e thin_col/test2_ -es 2 -stop 5000
#./poff_no_graph -l sand3.conf -s thin_columnv.sc -e thin_col/test3_ -es 2 -stop 5000
#./poff_no_graph -l sand4.conf -s thin_columnv.sc -e thin_col/test4_ -es 2 -stop 5000
#./poff_no_graph -l test_snow_ap1.conf -s thin_columnv.sc -e thin_col/testsnow1_ -es 2 -stop 5000
#./poff_no_graph -l test_snow_ap2.conf -s thin_columnv.sc -e thin_col/testsnow2_ -es 2 -stop 5000
#./poff_no_graph -l test_goo_ap1.conf -s thin_columnv.sc -e thin_col/testgoo1_ -es 2 -stop 5000
#./poff_no_graph -l test_goo_ap2.conf -s thin_columnv.sc -e thin_col/testgoo2_ -es 2 -stop 5000
#./poff_no_graph -l sand4.conf -s thin_columnv.sc -e thin_col/test4_ -es 20 -stop 50000

#mkdir thin_col_more_part/
#./poff_no_graph -l sand0.conf -s thin_columnv_more_part.sc -e thin_col_more_part/test0_ -es 10 -stop 2000
#./poff_no_graph -l sand-1.conf -s thin_columnv_more_part.sc -e thin_col_more_part/test-1_ -es 10 -stop 2000
#./poff_no_graph -l sand-2.conf -s thin_columnv_more_part.sc -e thin_col_more_part/test-2_ -es 10 -stop 2000
#./poff_no_graph -l sand-3.conf -s thin_columnv_more_part.sc -e thin_col_more_part/test-3_ -es 10 -stop 2000
#./poff_no_graph -l sand-4.conf -s thin_columnv_more_part.sc -e thin_col_more_part/test-4_ -es 10 -stop 2000
#./poff_no_graph -l sand1.conf -s thin_columnv_more_part.sc -e thin_col_more_part/test1_ -es 10 -stop 2000
#./poff_no_graph -l sand2.conf -s thin_columnv_more_part.sc -e thin_col_more_part/test2_ -es 10 -stop 2000
#./poff_no_graph -l sand3.conf -s thin_columnv_more_part.sc -e thin_col_more_part/test3_ -es 10 -stop 2000
#./poff_no_graph -l sand4.conf -s thin_columnv_more_part.sc -e thin_col_more_part/test4_ -es 10 -stop 2000
#./poff_no_graph -l test_snow_ap1.conf -s thin_columnv_more_part.sc -e thin_col_more_part/testsnow1_ -es 10 -stop 2000
#./poff_no_graph -l test_snow_ap2.conf -s thin_columnv_more_part.sc -e thin_col_more_part/testsnow2_ -es 10 -stop 2000
#./poff_no_graph -l test_goo_ap1.conf -s thin_columnv_more_part.sc -e thin_col_more_part/testgoo1_ -es 10 -stop 2000
#./poff_no_graph -l test_goo_ap2.conf -s thin_columnv_more_part.sc -e thin_col_more_part/testgoo2_ -es 10 -stop 2000
#
#mkdir falling_cube_cylinder_more_part/
#./poff_no_graph -l sand0.conf -s falling_cube_cylinderh_more_part.sc -e falling_cube_cylinder_more_part/test0_ -es 10 -stop 2000
##./poff_no_graph -l sand-1.conf -s falling_cube_cylinderh_more_part.sc -e falling_cube_cylinder_more_part/test-1_ -es 10 -stop 2000
#./poff_no_graph -l sand-2.conf -s falling_cube_cylinderh_more_part.sc -e falling_cube_cylinder_more_part/test-2_ -es 10 -stop 2000
#./poff_no_graph -l sand-3.conf -s falling_cube_cylinderh_more_part.sc -e falling_cube_cylinder_more_part/test-3_ -es 10 -stop 2000
#./poff_no_graph -l sand-4.conf -s falling_cube_cylinderh_more_part.sc -e falling_cube_cylinder_more_part/test-4_ -es 10 -stop 2000
##./poff_no_graph -l sand1.conf -s falling_cube_cylinderh_more_part.sc -e falling_cube_cylinder_more_part/test1_ -es 10 -stop 2000
#./poff_no_graph -l sand2.conf -s falling_cube_cylinderh_more_part.sc -e falling_cube_cylinder_more_part/test2_ -es 10 -stop 2000
#./poff_no_graph -l sand3.conf -s falling_cube_cylinderh_more_part.sc -e falling_cube_cylinder_more_part/test3_ -es 10 -stop 2000
#./poff_no_graph -l sand4.conf -s falling_cube_cylinderh_more_part.sc -e falling_cube_cylinder_more_part/test4_ -es 10 -stop 2000
#./poff_no_graph -l test_snow_ap1.conf -s falling_cube_cylinderh_more_part.sc -e falling_cube_cylinder_more_part/testsnow1_ -es 10 -stop 2000
#./poff_no_graph -l test_snow_ap2.conf -s falling_cube_cylinderh_more_part.sc -e falling_cube_cylinder_more_part/testsnow2_ -es 10 -stop 2000
#./poff_no_graph -l test_goo_ap1.conf -s falling_cube_cylinderh_more_part.sc -e falling_cube_cylinder_more_part/testgoo1_ -es 10 -stop 2000
#./poff_no_graph -l test_goo_ap2.conf -s falling_cube_cylinderh_more_part.sc -e falling_cube_cylinder_more_part/testgoo2_ -es 10 -stop 2000


#mkdir thin_col_small_dt/
#./poff_no_graph -l sand0.conf -s thin_columnv.sc -e thin_col_small_dt/test0_ -es 10 -stop 20000
#./poff_no_graph -l sand-1.conf -s thin_columnv.sc -e thin_col_small_dt/test-1_ -es 10 -stop 20000
#./poff_no_graph -l sand-2.conf -s thin_columnv.sc -e thin_col_small_dt/test-2_ -es 10 -stop 20000
#./poff_no_graph -l sand-3.conf -s thin_columnv.sc -e thin_col_small_dt/test-3_ -es 10 -stop 20000
#./poff_no_graph -l sand-4.conf -s thin_columnv.sc -e thin_col_small_dt/test-4_ -es 10 -stop 20000
#./poff_no_graph -l sand1.conf -s thin_columnv.sc -e thin_col_small_dt/test1_ -es 10 -stop 20000
#./poff_no_graph -l sand2.conf -s thin_columnv.sc -e thin_col_small_dt/test2_ -es 10 -stop 20000
#./poff_no_graph -l sand3.conf -s thin_columnv.sc -e thin_col_small_dt/test3_ -es 10 -stop 20000
#./poff_no_graph -l sand4.conf -s thin_columnv.sc -e thin_col_small_dt/test4_ -es 10 -stop 20000
#./poff_no_graph -l test_snow_ap1.conf -s thin_columnv.sc -e thin_col_small_dt/testsnow1_ -es 10 -stop 20000
#./poff_no_graph -l test_snow_ap2.conf -s thin_columnv.sc -e thin_col_small_dt/testsnow2_ -es 10 -stop 20000
#./poff_no_graph -l test_goo_ap1.conf -s thin_columnv.sc -e thin_col_small_dt/testgoo1_ -es 10 -stop 20000
#./poff_no_graph -l test_goo_ap2.conf -s thin_columnv.sc -e thin_col_small_dt/testgoo2_ -es 10 -stop 20000

#mkdir rotating_box/
#./poff_no_graph -l sand0.conf -s rotating_box.sc -e rotating_box/test0_ -es 4 -stop 5000
#./poff_no_graph -l sand-1.conf -s rotating_box.sc -e rotating_box/test-1_ -es 4 -stop 5000
#./poff_no_graph -l sand-2.conf -s rotating_box.sc -e rotating_box/test-2_ -es 4 -stop 5000
#./poff_no_graph -l sand-3.conf -s rotating_box.sc -e rotating_box/test-3_ -es 4 -stop 5000
#./poff_no_graph -l sand-4.conf -s rotating_box.sc -e rotating_box/test-4_ -es 4 -stop 5000
#./poff_no_graph -l sand1.conf -s rotating_box.sc -e rotating_box/test1_ -es 4 -stop 5000
#./poff_no_graph -l sand2.conf -s rotating_box.sc -e rotating_box/test2_ -es 4 -stop 5000
#./poff_no_graph -l sand3.conf -s rotating_box.sc -e rotating_box/test3_ -es 4 -stop 5000
#./poff_no_graph -l sand4.conf -s rotating_box.sc -e rotating_box/test4_ -es 4 -stop 5000
#./poff_no_graph -l test_snow_ap1.conf -s rotating_box.sc -e rotating_box/testsnow1_ -es 4 -stop 5000
#./poff_no_graph -l test_snow_ap2.conf -s rotating_box.sc -e rotating_box/testsnow2_ -es 4 -stop 5000
#./poff_no_graph -l test_goo_ap1.conf -s rotating_box.sc -e rotating_box/testgoo1_ -es 4 -stop 5000
#./poff_no_graph -l test_goo_ap2.conf -s rotating_box.sc -e rotating_box/testgoo2_ -es 4 -stop 5000
#
#mkdir corner/
#./poff_no_graph -l sand0.conf -s corner.sc -e corner/test0_ -es 4 -stop 5000
#./poff_no_graph -l sand-1.conf -s corner.sc -e corner/test-1_ -es 4 -stop 5000
#./poff_no_graph -l sand-2.conf -s corner.sc -e corner/test-2_ -es 4 -stop 5000
#./poff_no_graph -l sand-3.conf -s corner.sc -e corner/test-3_ -es 4 -stop 5000
#./poff_no_graph -l sand-4.conf -s corner.sc -e corner/test-4_ -es 4 -stop 5000
#./poff_no_graph -l sand1.conf -s corner.sc -e corner/test1_ -es 4 -stop 5000
#./poff_no_graph -l sand2.conf -s corner.sc -e corner/test2_ -es 4 -stop 5000
#./poff_no_graph -l sand3.conf -s corner.sc -e corner/test3_ -es 4 -stop 5000
#./poff_no_graph -l sand4.conf -s corner.sc -e corner/test4_ -es 4 -stop 5000
#./poff_no_graph -l test_snow_ap1.conf -s corner.sc -e corner/testsnow1_ -es 4 -stop 5000
#./poff_no_graph -l test_snow_ap2.conf -s corner.sc -e corner/testsnow2_ -es 4 -stop 5000
#./poff_no_graph -l test_goo_ap1.conf -s corner.sc -e corner/testgoo1_ -es 4 -stop 5000
#./poff_no_graph -l test_goo_ap2.conf -s corner.sc -e corner/testgoo2_ -es 4 -stop 5000
#

#rm -r test_snow0
#mkdir test_snow0
#./poff_no_graph -l test_snow.conf -s rotating_boxh.sc -e test_snow0/test -es 4 -stop 4000

#rm -r test_snow_ap1
#mkdir test_snow_ap1
#./poff_no_graph -l test_snow_ap1.conf -s rotating_boxr.sc -e test_snow_ap1/testr_ -es 4 -stop 4000
#./poff_no_graph -l test_snow_ap1.conf -s rotating_boxh.sc -e test_snow_ap1/testh_ -es 4 -stop 4000
#./poff_no_graph -l test_snow_ap1.conf -s rotating_boxd.sc -e test_snow_ap1/testd_ -es 4 -stop 4000
#./poff_no_graph -l test_snow_ap1.conf -s rotating_boxv.sc -e test_snow_ap1/testv_ -es 4 -stop 4000

#rm -r test_snow_ap2
#mkdir test_snow_ap2
#./poff_no_graph -l test_snow_ap2.conf -s rotating_boxr.sc -e test_snow_ap2/testr_ -es 4 -stop 4000
#./poff_no_graph -l test_snow_ap2.conf -s rotating_boxh.sc -e test_snow_ap2/testh_ -es 4 -stop 4000
#./poff_no_graph -l test_snow_ap2.conf -s rotating_boxd.sc -e test_snow_ap2/testd_ -es 4 -stop 4000
#./poff_no_graph -l test_snow_ap2.conf -s rotating_boxv.sc -e test_snow_ap2/testv_ -es 4 -stop 4000
#
#rm -r test_sand0
#mkdir test_sand0
#./poff_no_graph -l test_sand.conf -s rotating_boxh.sc -e test_sand0/test -es 4 -stop 4000
#
#rm -r test_sand_ap1
#mkdir test_sand_ap1
#./poff_no_graph -l test_sand_ap1.conf -s rotating_boxr.sc -e test_sand_ap1/testr_ -es 4 -stop 4000
#./poff_no_graph -l test_sand_ap1.conf -s rotating_boxh.sc -e test_sand_ap1/testh_ -es 4 -stop 4000
#./poff_no_graph -l test_sand_ap1.conf -s rotating_boxd.sc -e test_sand_ap1/testd_ -es 4 -stop 4000
#./poff_no_graph -l test_sand_ap1.conf -s rotating_boxv.sc -e test_sand_ap1/testv_ -es 4 -stop 4000
#
#rm -r test_sand_ap2
#mkdir test_sand_ap2
#./poff_no_graph -l test_sand_ap2.conf -s rotating_boxr.sc -e test_sand_ap2/testr_ -es 4 -stop 4000
#./poff_no_graph -l test_sand_ap2.conf -s rotating_boxh.sc -e test_sand_ap2/testh_ -es 4 -stop 4000
#./poff_no_graph -l test_sand_ap2.conf -s rotating_boxd.sc -e test_sand_ap2/testd_ -es 4 -stop 4000
#./poff_no_graph -l test_sand_ap2.conf -s rotating_boxv.sc -e test_sand_ap2/testv_ -es 4 -stop 4000
#
#rm -r test_goo0
#mkdir test_goo0
#./poff_no_graph -l test_goo.conf -s rotating_boxh.sc -e test_goo0/test -es 4 -stop 4000
#
#rm -r test_goo_ap1
#mkdir test_goo_ap1
#./poff_no_graph -l test_goo_ap1.conf -s rotating_boxr.sc -e test_goo_ap1/testr_ -es 4 -stop 4000
#./poff_no_graph -l test_goo_ap1.conf -s rotating_boxh.sc -e test_goo_ap1/testh_ -es 4 -stop 4000
#./poff_no_graph -l test_goo_ap1.conf -s rotating_boxd.sc -e test_goo_ap1/testd_ -es 4 -stop 4000
#./poff_no_graph -l test_goo_ap1.conf -s rotating_boxv.sc -e test_goo_ap1/testv_ -es 4 -stop 4000
#
#rm -r test_goo_ap2
#mkdir test_goo_ap2
#./poff_no_graph -l test_goo_ap2.conf -s rotating_boxr.sc -e test_goo_ap2/testr_ -es 4 -stop 4000
#./poff_no_graph -l test_goo_ap2.conf -s rotating_boxh.sc -e test_goo_ap2/testh_ -es 4 -stop 4000
#./poff_no_graph -l test_goo_ap2.conf -s rotating_boxd.sc -e test_goo_ap2/testd_ -es 4 -stop 4000
#./poff_no_graph -l test_goo_ap2.conf -s rotating_boxv.sc -e test_goo_ap2/testv_ -es 4 -stop 4000


#rm -r test_snow0
#mkdir test_snow0
#./poff_no_graph -l test_snow.conf -s rotating_planesh.sc -e test_snow0/test -es 4 -stop 4000
#
#rm -r test_snow_ap1
#mkdir test_snow_ap1
#./poff_no_graph -l test_snow_ap1.conf -s rotating_planesr.sc -e test_snow_ap1/testr_ -es 4 -stop 4000
#./poff_no_graph -l test_snow_ap1.conf -s rotating_planesh.sc -e test_snow_ap1/testh_ -es 4 -stop 4000
#./poff_no_graph -l test_snow_ap1.conf -s rotating_planesd.sc -e test_snow_ap1/testd_ -es 4 -stop 4000
#./poff_no_graph -l test_snow_ap1.conf -s rotating_planesv.sc -e test_snow_ap1/testv_ -es 4 -stop 4000
#
#rm -r test_snow_ap2
#mkdir test_snow_ap2
#./poff_no_graph -l test_snow_ap2.conf -s rotating_planesr.sc -e test_snow_ap2/testr_ -es 4 -stop 4000
#./poff_no_graph -l test_snow_ap2.conf -s rotating_planesh.sc -e test_snow_ap2/testh_ -es 4 -stop 4000
#./poff_no_graph -l test_snow_ap2.conf -s rotating_planesd.sc -e test_snow_ap2/testd_ -es 4 -stop 4000
#./poff_no_graph -l test_snow_ap2.conf -s rotating_planesv.sc -e test_snow_ap2/testv_ -es 4 -stop 4000

#rm -r test_sand0
#mkdir test_sand0
#./poff_no_graph -l test_sand.conf -s rotating_planesh.sc -e test_sand0/test -es 4 -stop 4000
#
#rm -r test_sand_ap1
#mkdir test_sand_ap1
#./poff_no_graph -l test_sand_ap1.conf -s rotating_planesr.sc -e test_sand_ap1/testr_ -es 4 -stop 4000
#./poff_no_graph -l test_sand_ap1.conf -s rotating_planesh.sc -e test_sand_ap1/testh_ -es 4 -stop 4000
#./poff_no_graph -l test_sand_ap1.conf -s rotating_planesd.sc -e test_sand_ap1/testd_ -es 4 -stop 4000
#./poff_no_graph -l test_sand_ap1.conf -s rotating_planesv.sc -e test_sand_ap1/testv_ -es 4 -stop 4000
#
#rm -r test_sand_ap2
#mkdir test_sand_ap2
#./poff_no_graph -l test_sand_ap2.conf -s rotating_planesr.sc -e test_sand_ap2/testr_ -es 4 -stop 4000
#./poff_no_graph -l test_sand_ap2.conf -s rotating_planesh.sc -e test_sand_ap2/testh_ -es 4 -stop 4000
#./poff_no_graph -l test_sand_ap2.conf -s rotating_planesd.sc -e test_sand_ap2/testd_ -es 4 -stop 4000
#./poff_no_graph -l test_sand_ap2.conf -s rotating_planesv.sc -e test_sand_ap2/testv_ -es 4 -stop 4000


#rm -r test_goo0
#mkdir test_goo0
#./poff_no_graph -l test_goo.conf -s rotating_planesh.sc -e test_goo0/test -es 40 -stop 40000
#
#rm -r test_goo_ap1
#mkdir test_goo_ap1
#./poff_no_graph -l test_goo_ap1.conf -s rotating_planesr.sc -e test_goo_ap1/testr_ -es 40 -stop 40000
#./poff_no_graph -l test_goo_ap1.conf -s rotating_planesh.sc -e test_goo_ap1/testh_ -es 40 -stop 40000
#./poff_no_graph -l test_goo_ap1.conf -s rotating_planesd.sc -e test_goo_ap1/testd_ -es 40 -stop 40000
#./poff_no_graph -l test_goo_ap1.conf -s rotating_planesv.sc -e test_goo_ap1/testv_ -es 40 -stop 40000
#
#rm -r test_goo_ap2
#mkdir test_goo_ap2
#./poff_no_graph -l test_goo_ap2.conf -s rotating_planesr.sc -e test_goo_ap2/testr_ -es 40 -stop 40000
#./poff_no_graph -l test_goo_ap2.conf -s rotating_planesh.sc -e test_goo_ap2/testh_ -es 40 -stop 40000
#./poff_no_graph -l test_goo_ap2.conf -s rotating_planesd.sc -e test_goo_ap2/testd_ -es 40 -stop 40000
#./poff_no_graph -l test_goo_ap2.conf -s rotating_planesv.sc -e test_goo_ap2/testv_ -es 40 -stop 40000


#rm -r test_sand_mixer2
#mkdir test_sand_mixer2
#./poff_no_graph -l sand3.conf -s rotating_planesr.sc -e test_sand_mixer2/testr_ -es 4 -stop 4000
#./poff_no_graph -l sand3.conf -s rotating_planesh.sc -e test_sand_mixer2/testh_ -es 4 -stop 4000
#./poff_no_graph -l sand3.conf -s rotating_planesd.sc -e test_sand_mixer2/testd_ -es 4 -stop 4000
#./poff_no_graph -l sand3.conf -s rotating_planesd2.sc -e test_sand_mixer2/testd2_ -es 4 -stop 4000
#./poff_no_graph -l sand3.conf -s rotating_planesv.sc -e test_sand_mixer2/testv_ -es 4 -stop 4000
#./poff_no_graph -l sand3.conf -s rotating_planesv2.sc -e test_sand_mixer2/testv2_ -es 4 -stop 4000

#rm -r test_wood
#mkdir test_wood
#./poff_no_graph -l test_wood2.sc -s broken_cuboidv.sc -e test_wood/testv_ -es 10 -stop 15000
#./poff_no_graph -l test_wood2.sc -s broken_cuboidh.sc -e test_wood/testh_ -es 10 -stop 15000
#./poff_no_graph -l test_wood2.sc -s broken_cuboidd.sc -e test_wood/testd_ -es 10 -stop 15000

#./poff_no_graph -s column_testh.sc -l test_sand_fall.conf -e test/testh_ -stop 1000
#./poff_no_graph -s column_testv.sc -l test_sand_fall.conf -e test/testv_ -stop 1000
#./poff_no_graph -s column_testd.sc -l test_sand_fall.conf -e test/testd_ -stop 1000

mkdir falling_sub
./poff_no_graph -s falling_cube_cylinder_testv.sc -l test_sand_fall.conf -e falling_sub/testv_ -stop 1000
./poff_no_graph -s falling_cube_cylinder_testh.sc -l test_sand_fall.conf -e falling_sub/testh_ -stop 1000
./poff_no_graph -s falling_cube_cylinder_testd.sc -l test_sand_fall.conf -e falling_sub/testd_ -stop 1000

mkdir stretch_sub
./poff_no_graph -s broken_cuboid_subh.sc -l test_sub.conf -e stretch_sub/testh_ -es 10 -stop 15000
./poff_no_graph -s broken_cuboid_subh.sc -l test_sub.conf -e stretch_sub/testh_ -es 10 -stop 15000
./poff_no_graph -s broken_cuboid_subh.sc -l test_sub.conf -e stretch_sub/testh_ -es 10 -stop 15000
