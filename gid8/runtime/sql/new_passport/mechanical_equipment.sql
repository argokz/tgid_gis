select	
		pts.tblName, 				
		sl.beginPts,
		sl.endPts,
		pts.externalID,
		pts.armatureType,
		pts.designType,
		pts.constructionType,
		pts.material,
		pts.diametr,		
		count(diametr) count_obj	
	from pts_objects pts
	left join vtSortLinesUchastok sl ON sl.id = pts.id_vts 
	where sl.$msrs$ = $fileID$ and pts.externalID is not NULL
	group by 
		pts.tblName, 
		sl.beginPts,
		sl.endPts,		
		pts.externalID,
		pts.armatureType,
		pts.designType,
		pts.constructionType,
		pts.material,
		pts.diametr