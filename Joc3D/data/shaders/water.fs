
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform float u_time;

void main()
{
	vec2 uv = v_uv;
	uv = v_world_position.xz * 0.01;
	uv.x += u_time * 0.10 ;
	vec4 color = texture2D( u_texture, uv );

	uv = v_world_position.xz * 0.01;
	uv.x += u_time * 0.10 ;

	color = color * 0.5 + 0.5 * texture2D( u_texture, uv);

	gl_FragColor = u_color * color;
	//gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
