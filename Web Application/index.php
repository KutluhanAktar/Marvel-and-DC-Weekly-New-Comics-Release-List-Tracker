<?php

// Define the 'comics' class to print requested comics release lists:
class comics{
	
	public function get_this_week_releases(){
		// Get this week's released comics list from the Shortboxed comics API.
		$new_comics = json_decode(file_get_contents("https://api.shortboxed.com/comics/v1/new"));
		// Print new comics from selected publishers.
		$date;
		echo "%";
		foreach($new_comics->comics as $comic){
			if($comic->publisher == "MARVEL COMICS" || $comic->publisher == "DC COMICS" /*|| $comic->publisher == "IMAGE COMICS"*/){
				echo $comic->publisher.", ".$comic->title." **** ";	
				$date = $comic->release_date;
			}
		}
		echo "%".$date."%";
	}
	
	public function get_previous_week_releases(){
		// Get previous week's released comics list from the Shortboxed comics API.
		$previous_comics = json_decode(file_get_contents("https://api.shortboxed.com/comics/v1/previous"));
		// Print comics from selected publishers.
		$date;
		echo "%";
		foreach($previous_comics->comics as $comic){
			if($comic->publisher == "MARVEL COMICS" || $comic->publisher == "DC COMICS" /*|| $comic->publisher == "IMAGE COMICS"*/){
				echo $comic->publisher.", ".$comic->title." **** ";	
				$date = $comic->release_date;
			}
		}
		echo "%".$date."%";
	}
	
	public function get_next_week_releases(){
		// Get next week's comics list from the Shortboxed comics API.
		$next_comics = json_decode(file_get_contents("https://api.shortboxed.com/comics/v1/future"));
		// Print comics from selected publishers.
		$date;
		echo "%";
		foreach($next_comics->comics as $comic){
			if($comic->publisher == "MARVEL COMICS" || $comic->publisher == "DC COMICS" /*|| $comic->publisher == "IMAGE COMICS"*/){
				echo $comic->publisher.", ".$comic->title." **** ";	
				$date = $comic->release_date;
			}
		}
		echo "%".$date."%";
	}
}

// Define a new object for the 'comics' class:
$releases = new comics();
// Print the requested comics release list by the device - this week, previous week, or next week.
if(isset($_GET["q"]) && $_GET["q"] != ""){
	if ($_GET["q"] == "this") $releases->get_this_week_releases();
	if ($_GET["q"] == "previous") $releases->get_previous_week_releases();
	if ($_GET["q"] == "next") $releases->get_next_week_releases();
}

?>