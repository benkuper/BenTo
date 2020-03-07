function sendColors(colors, resolution)
{
	for(var i =0;i<resolution;i++)
	{
		local.send(colors[i][0]*254, colors[i][1]*254,colors[i][2]*254);
	}

	local.send(255);
}