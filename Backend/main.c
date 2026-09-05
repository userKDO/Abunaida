#include <stdio.h>
#include "player.h"

int main(void)
{
	PlayerContext ctx;
	printf("[MAIN] Старт тестирования бэкенда...\n");

	// 1. Инициализируем плеер и звуковую карту
	if (player_init(&ctx) != 0) 
	{
		printf("[MAIN] Ошибка инициализации плеера!\n");
		return 1;
	}

	// 2. Пробуем загрузить трек через FFmpeg
	int res = player_load(&ctx, "test.mp3");
	if (res != 0)
	{
		printf("[MAIN] Не удалось загрузить test.mp3, код ошибки: %d\n", res);
		player_uinit(&ctx);
		return 1;
	}

	// 3. Выводим результат успешного парсинга тегов/длительности файла
	printf("[MAIN] Успешно загружен: %s\n", ctx.current_track_path);
	printf("[MAIN] Длительность трека: %.2f секунд\n", ctx.total_duration);

	// 4. Очищаем память перед выходом
	player_uinit(&ctx);
	printf("[MAIN] Тест успешно пройден!\n");
	
	return 0;
}
