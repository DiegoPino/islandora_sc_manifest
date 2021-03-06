<?php

	function __generate_sc_manifest($object_pid, $from_drush = FALSE) {

		require_once ("SharedCanvasManifest.inc");
		require_once ("Canvas.inc");
		require_once ("utlfedoraobject.inc");

		global $base_url; 
		//$dsid = "JPGHIRES";

		$user = user_load(1);
		$fo = new UTLFedoraObject($user->name, $user->pass);

		$object = $fo->get_object($object_pid);

		$manifest = new SharedCanvasManifest("$base_url/islandora/object/$object_pid/datastream/SC/view", $object->label, "$base_url/islandora/object/$object_pid/datastream/MODS");
		
		// Get the pages ordered by sequence number
		module_load_include('inc', 'islandora_paged_content', 'includes/utilities');
		$pages = islandora_paged_content_get_pages($object);

		if ($from_drush) {
			echo "Processing $object_pid\n";
		}
		foreach($pages as $page) {

		    $page_pid = $page['pid'];

		    if ($from_drush) {
		    	echo "Processing Page: $page_pid\n";
			}
		    $page_object = $fo->get_object($page_pid);
		    $page_uri = "$base_url/islandora/object/$page_pid";
		    
		    $page_object_rels = $page_object['JP2']->relationships;
		    $width_rel = $page_object_rels->get('http://islandora.ca/ontology/relsext#', 'width');
		    $height_rel = $page_object_rels->get('http://islandora.ca/ontology/relsext#', 'height');
		    $width = $width_rel[0]['object']['value'];
		    $height = $height_rel[0]['object']['value'];

		    // this width and height is for the original JP2
		    // recalculate width and height based on fixed height of 2000 (which is the case for JPGHIRES
		    //$height = round($width * 2000/$height);
		    //$width = 2000;
		    
		    //$mimetype = $page_object[$dsid]->mimetype;
                    $mimetype = "image/jpg";
		    
		    //echo $mimetype . "\n";
		    //echo $width . "\n";
		    //echo $height . "\n";
		    //https://iiif.library.utoronto.ca/v2/paleography%3A2071/full/2000,/0/default.jpg

		    $metadata_uri = $page_uri . "/datastream/MODS/view";
		    $thumbnail_uri = $page_uri . "/datastream/TN/view";
		    $image_uri = $page_uri . "/datastream/TN/view";
		    $resource_uri = variable_get('islandora_sc_manifest_iiif_server') . "/$page_pid/full/2000,/0/default.jpg";

		    $canvas = new Canvas($page_uri, $page_pid, $page_object->label, $metadata_uri);
		    $canvas->setImage($thumbnail_uri, $image_uri, $resource_uri, $mimetype, $width, $height);
		    $manifest->addCanvas($canvas);

		    //print_r($canvas);
		    
		    //break;
		}
		
		//break;
		$json = $manifest->getJson();
		$fo->add_datastream($object, "SC", "SharedCanvas Manifest", "application/json", $json);
		drupal_set_message(t('SC Manifest created'));
	}
